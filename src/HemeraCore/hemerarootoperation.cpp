#include "hemerarootoperation_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QUuid>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include <HemeraCore/Literals>

#include <systemd/sd-daemon.h>

#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

Q_LOGGING_CATEGORY(LOG_HEMERA_ROOTOPERATION, "Hemera::RootOperation")

namespace Hemera {

int RootOperation::rootOperationMain(RootOperation *op)
{
    // Set everything up
    connect(op, &Operation::finished, [op] {
        // Done.
        if (op->isError()) {
            QString status = tr("Operation failed! %1: %2. Shutting down worker...").arg(op->errorName(), op->errorMessage());
            qCWarning(LOG_HEMERA_ROOTOPERATION) << status;
            sd_notifyf(0, "STATUS=%s", status.toLatin1().constData());
        } else {
            qCDebug(LOG_HEMERA_ROOTOPERATION) << tr("Operation completed successfully. Shutting down worker...");
            sd_notifyf(0, "STATUS=%s", tr("Operation completed successfully. Shutting down worker...").toLatin1().constData());
        }

        // Force early deletion, but let the event loop do its thing.
        QTimer::singleShot(1000, op, SLOT(deleteLater()));
    });

    connect(op, &QObject::destroyed, [] {
        QCoreApplication::instance()->quit();
    });

    // Notify systemd
    sd_notify(0, "READY=1");

    return QCoreApplication::instance()->exec();
}

void RootOperationPrivate::initServer()
{
    Q_Q(RootOperation);

    // Initialize random seed
    qsrand(QTime::currentTime().msec());

    // Check our socket activation, first of all
    if (Q_UNLIKELY(sd_listen_fds(0) != 1)) {
        q->setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::systemdError()),
                                RootOperation::tr("No or too many file descriptors received."));
        return;
    }

    // Create our client
    client = new QLocalSocket(q);

    QObject::connect(client.data(), &QLocalSocket::stateChanged, q, [this, q] (QLocalSocket::LocalSocketState socketState) {
        if (socketState == QLocalSocket::ConnectedState) {
            // Yeah.
            qCDebug(LOG_HEMERA_ROOTOPERATION) << RootOperation::tr("Client marked as connected.");
        } else if (socketState == QLocalSocket::UnconnectedState && !q->isFinished()) {
            // Ouch.
            qCWarning(LOG_HEMERA_ROOTOPERATION) << RootOperation::tr("Socket connection to root operation client was reset!")
                                                << client->error() << client->errorString();
            // Time to die!
            q->setFinishedWithError(Literals::literal(Literals::Errors::canceled()), client->errorString());
        }
    });

    // Data pipe to socket
    QObject::connect(client.data(), &QLocalSocket::readyRead, q, [this, q] {
        // go. we use QDataStream internally
        QByteArray data = client->readAll();
        QDataStream in(&data, QIODevice::ReadOnly);
        // Sockets may buffer several requests in case they're concurrent: unroll the whole DataStream.
        while (!in.atEnd()) {
            QByteArray binaryPayload;
            in >> binaryPayload;

            QJsonDocument document = QJsonDocument::fromBinaryData(binaryPayload);
            if (document.isNull() || !document.isObject()) {
                qCWarning(LOG_HEMERA_ROOTOPERATION) << RootOperation::tr("Got corrupted data from Operation!");
                continue;
            }

            // Before processing, check everything is fine
            QJsonObject payload = QJsonDocument::fromBinaryData(binaryPayload).object();

            // Are we creating/requesting the instance?
            if (payload.value(QStringLiteral("type")).toString() == QStringLiteral("NetworkTransparentOperationRequest")) {
                // We have to handle this here
                if (!instanceId.isEmpty()) {
                    // Can't do that!
                    qCWarning(LOG_HEMERA_ROOTOPERATION) << RootOperation::tr("Requested another instance while another one was running!");
                    QJsonObject ret;
                    ret.insert(QStringLiteral("type"), QStringLiteral("NetworkTransparentOperationInstance"));
                    ret.insert(QStringLiteral("operationId"), q->id());
                    ret.insert(QStringLiteral("requestToken"), payload.value(QStringLiteral("requestToken")));
                    ret.insert(QStringLiteral("success"), false);
                    ret.insert(QStringLiteral("errorName"), Literals::literal(Literals::Errors::badRequest()));
                    ret.insert(QStringLiteral("errorMessage"), RootOperation::tr("This operation supports only one instance at a time."));
                    Q_EMIT q->newOutgoingPayload(ret);
                    continue;
                }

                // Set parameters
                q->setParameters(payload.value(QStringLiteral("parameters")).toObject());

                // Create instance
                instanceId = QString::number(qrand() % 99999);
                q->setInstanceId(instanceId);
                QJsonObject ret;
                ret.insert(QStringLiteral("type"), QStringLiteral("NetworkTransparentOperationInstance"));
                ret.insert(QStringLiteral("operationId"), q->id());
                ret.insert(QStringLiteral("requestToken"), payload.value(QStringLiteral("requestToken")));
                ret.insert(QStringLiteral("success"), true);
                ret.insert(QStringLiteral("instanceId"), instanceId);
                Q_EMIT q->newOutgoingPayload(ret);
                continue;
            }

            // Verify instance id
            if (payload.value(QStringLiteral("instanceId")).toString() != instanceId) {
                qCWarning(LOG_HEMERA_ROOTOPERATION) << RootOperation::tr("Got request with wrong instance id!");
                continue;
            }

            // Pass the payload over
            q->processIncomingPayload(payload);
        }
    });

    QObject::connect(q, &NetworkTransparentOperation::newOutgoingPayload, [this] (const QJsonObject &payload) {
        if (client.isNull()) {
            return;
        }

        // Serialize
        QJsonDocument doc(payload);

        QByteArray actualPayload;
        QDataStream out(&actualPayload, QIODevice::WriteOnly);
        out << doc.toBinaryData();

        client->write(actualPayload);
    });

    if (Q_UNLIKELY(!client->setSocketDescriptor(SD_LISTEN_FDS_START + 0))) {
        q->setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::systemdError()),
                                RootOperation::tr("Could not bind to systemd's socket for operation."));
        return;
    }
}

RootOperation::RootOperation(const QString &id, QObject* parent)
    : NetworkTransparentOperation(*new RootOperationPrivate(this), id, parent)
{
    // Init server
    QTimer::singleShot(0, this, SLOT(initServer()));
}

RootOperation::~RootOperation()
{
}

}

#include "moc_hemerarootoperation.cpp"
