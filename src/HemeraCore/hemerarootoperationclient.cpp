#include "hemerarootoperationclient_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTimer>
#include <QtCore/QUuid>

#include <QtNetwork/QLocalSocket>

#include <HemeraCore/Literals>

Q_LOGGING_CATEGORY(LOG_HEMERA_ROOTOPERATIONCLIENT, "Hemera::RootOperationClient")

namespace Hemera {

void RootOperationClientPrivate::initClient()
{
    Q_Q(RootOperationClient);

    socket = new QLocalSocket(q);

    QObject::connect(socket.data(), &QLocalSocket::stateChanged, q, [this, q] (QLocalSocket::LocalSocketState socketState) {
        if (socketState == QLocalSocket::ConnectedState) {
            // Yeah.
            qCDebug(LOG_HEMERA_ROOTOPERATIONCLIENT) << RootOperationClient::tr("Connected to root operation successfully! Setting transport ready.");

            // Run the event loop for the device to open.
            QTimer::singleShot(0, q, SLOT(setTransportReady()));
        } else if (socketState == QLocalSocket::UnconnectedState && !q->isFinished()) {
            // Ouch.
            qCWarning(LOG_HEMERA_ROOTOPERATIONCLIENT) << RootOperationClient::tr("Socket connection to root operation was reset!")
                                                      << socket->error() << socket->errorString();
            q->setTransportError(Literals::literal(Literals::Errors::failedRequest()), socket->errorString());
        }
    });

    // Data pipe to socket
    QObject::connect(socket.data(), &QLocalSocket::readyRead, q, [this, q] {
        // go. we use QDataStream internally
        QByteArray data = socket->readAll();
        QDataStream in(&data, QIODevice::ReadOnly);
        // Sockets may buffer several requests in case they're concurrent: unroll the whole DataStream.
        while (!in.atEnd()) {
            QByteArray binaryPayload;
            in >> binaryPayload;

            QJsonDocument document = QJsonDocument::fromBinaryData(binaryPayload);
            if (document.isNull() || !document.isObject()) {
                qCWarning(LOG_HEMERA_ROOTOPERATIONCLIENT) << RootOperationClient::tr("Got corrupted data from Operation!");
                continue;
            }

            q->processIncomingPayload(QJsonDocument::fromBinaryData(binaryPayload).object());
        }
    });

    QObject::connect(q, &NetworkTransparentOperationClient::newOutgoingPayload, [this] (const QJsonObject &payload) {
        if (socket.isNull()) {
            return;
        }

        // Serialize
        QJsonDocument doc(payload);

        QByteArray actualPayload;
        QDataStream out(&actualPayload, QIODevice::WriteOnly);
        out << doc.toBinaryData();

        socket->write(actualPayload);
    });

    qCDebug(LOG_HEMERA_ROOTOPERATIONCLIENT) << RootOperationClient::tr("Connecting to root operation gateway") << q->remoteOperationId();
    socket->connectToServer(QStringLiteral("/run/hemera/rootoperations/%1").arg(q->remoteOperationId()));
}

RootOperationClient::RootOperationClient(const QString &id, const QJsonObject &parameters, ExecutionOptions options, QObject *parent)
    : RootOperationClient(*new RootOperationClientPrivate(this), id, parameters, options, parent)
{
}

RootOperationClient::RootOperationClient(RootOperationClientPrivate &dd, const QString &id,
                                         const QJsonObject &parameters, ExecutionOptions options, QObject *parent)
    : NetworkTransparentOperationClient(dd, id, options, parameters, parent)
{
    // Init client
    QTimer::singleShot(0, this, SLOT(initClient()));
}

RootOperationClient::~RootOperationClient()
{
}

}

#include "moc_hemerarootoperationclient.cpp"
