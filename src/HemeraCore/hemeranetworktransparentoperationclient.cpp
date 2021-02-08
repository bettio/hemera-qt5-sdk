#include "hemeranetworktransparentoperationclient_p.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

#include <HemeraCore/Literals>

Q_LOGGING_CATEGORY(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT, "Hemera::NetworkTransparentOperationClient")

static quint32 ntoc_internal_id_sequential = 0;

namespace Hemera {

NetworkTransparentOperationClient::NetworkTransparentOperationClient(const QString& id, const QJsonObject &parameters, QObject* parent)
    : NetworkTransparentOperationClient(id, NoOptions, parameters, parent)
{
}

NetworkTransparentOperationClient::NetworkTransparentOperationClient(const QString& id, ExecutionOptions options, const QJsonObject &parameters, QObject* parent)
    : NetworkTransparentOperationClient(*new NetworkTransparentOperationClientPrivate(this), id, options, parameters, parent)
{
}

NetworkTransparentOperationClient::NetworkTransparentOperationClient(NetworkTransparentOperationClientPrivate &dd, const QString& id,
                                                                     ExecutionOptions options, const QJsonObject &parameters, QObject* parent)
    : OperationWithProgress(dd, options, parent)
{
    Q_D(NetworkTransparentOperationClient);

    d->remoteOperationId = id;
    d->parameters = parameters;
}

NetworkTransparentOperationClient::~NetworkTransparentOperationClient()
{
}

QString NetworkTransparentOperationClient::instanceId() const
{
    Q_D(const NetworkTransparentOperationClient);
    return d->instanceId;
}

bool NetworkTransparentOperationClient::isConnected() const
{
    Q_D(const NetworkTransparentOperationClient);
    return d->connected;
}

QString NetworkTransparentOperationClient::remoteOperationId() const
{
    Q_D(const NetworkTransparentOperationClient);
    return d->remoteOperationId;
}

QJsonObject NetworkTransparentOperationClient::returnValue() const
{
    Q_D(const NetworkTransparentOperationClient);
    return d->returnValue;
}

void NetworkTransparentOperationClient::setTransportError(const QString& name, const QString& message)
{
    Q_D(NetworkTransparentOperationClient);

    d->transportErrorName = name;
    d->transportErrorMessage = message;

    if (!isStarted()) {
        qCWarning(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << "Failing before start!";
        // It's over.
        setFinishedWithError(name, message);
    }
}

void NetworkTransparentOperationClient::setTransportReady()
{
    Q_D(NetworkTransparentOperationClient);

    if (!d->transportReady) {
        d->transportReady = true;
        Q_EMIT transportReady();
    }
}

void NetworkTransparentOperationClient::startImpl()
{
    Q_D(NetworkTransparentOperationClient);

    if (isStarted()) {
        qCWarning(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("startImpl called, but operation already started!");
        return;
    }

    if (!d->transportErrorName.isEmpty()) {
        // Transport is dead
        qCWarning(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << "Failing due to early transport error";
        setFinishedWithError(d->transportErrorName, d->transportErrorMessage);
        return;
    }

    if (!d->transportReady) {
        qCDebug(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Requested client start with transport not ready. Waiting...");
        connect(this, &NetworkTransparentOperationClient::transportReady, this, &NetworkTransparentOperationClient::startImpl);
        return;
    }

    qCDebug(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Starting instance handshake.");

    // Connect to operation
    if (ntoc_internal_id_sequential == 0) {
        // Yes, the random seed might not be initialized. But actually we don't really care, as it's just a token
        // and conflicts cannot happen if not inside the same process.
        ntoc_internal_id_sequential = qrand() % 99999;
    }
    ++ntoc_internal_id_sequential;

    // Prepare a request payload
    d->requestToken = ntoc_internal_id_sequential;

    QJsonObject data;
    data.insert(QStringLiteral("type"), QStringLiteral("NetworkTransparentOperationRequest"));
    data.insert(QStringLiteral("operationId"), d->remoteOperationId);
    data.insert(QStringLiteral("requestToken"), d->requestToken);
    data.insert(QStringLiteral("parameters"), d->parameters);

    Q_EMIT newOutgoingPayload(data);

    // Now we wait. Watch out - setStarted will be called by our payload receiver when the remote operation starts.
}

void NetworkTransparentOperationClient::processIncomingPayload(const QJsonObject& payload)
{
    Q_D(NetworkTransparentOperationClient);

    if (isFinished()) {
        qCDebug(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Got incoming payload, but the operation is already finished. Ignoring...");
        return;
    }

    if (payload.value(QStringLiteral("type")).toString() == QStringLiteral("NetworkTransparentOperationStatus") &&
        payload.value(QStringLiteral("instanceId")).toString() == d->instanceId && payload.value(QStringLiteral("status")).isObject()) {
        // Sync
        QJsonObject status = payload.value(QStringLiteral("status")).toObject();
        if (status.value(QStringLiteral("progress")).toInt() != d->progress) {
            setProgress(status.value(QStringLiteral("progress")).toInt());
        }
        if (status.value(QStringLiteral("statusMessage")).toString() != d->statusMessage) {
            setStatusMessage(status.value(QStringLiteral("statusMessage")).toString());
        }
        if (status.value(QStringLiteral("started")).toBool() && !d->started) {
            d->setStarted();
        }
        if (status.value(QStringLiteral("finished")).toBool() && !d->finished) {
            d->returnValue = status.value(QStringLiteral("returnValue")).toObject();

            if (status.value(QStringLiteral("error")).toBool()) {
                setFinishedWithError(status.value(QStringLiteral("errorName")).toString(), status.value(QStringLiteral("errorMessage")).toString());
            } else {
                setFinished();
            }

            qCDebug(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Remote operation finished execution.");
        }
    } else if (payload.value(QStringLiteral("type")).toString() == QStringLiteral("NetworkTransparentOperationInstance") &&
               !isConnected() && payload.value(QStringLiteral("operationId")).toString() == d->remoteOperationId) {
        if (!payload.value(QStringLiteral("success")).toBool()) {
            qCWarning(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Creation of a remote instance failed!")
                                                                    << payload.value(QStringLiteral("errorMessage")).toString();
            setFinishedWithError(payload.value(QStringLiteral("errorName")).toString(), payload.value(QStringLiteral("errorMessage")).toString());
            return;
        }

        d->instanceId = payload.value(QStringLiteral("instanceId")).toString();

        qCDebug(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Remote instance created with ID %1").arg(d->instanceId);
        d->connected = true;
        Q_EMIT connected();

        // Send a start request now.
        QJsonObject data;
        data.insert(QStringLiteral("type"), QStringLiteral("NetworkTransparentOperationCommand"));
        data.insert(QStringLiteral("instanceId"), d->instanceId);
        data.insert(QStringLiteral("command"), QStringLiteral("start"));

        Q_EMIT newOutgoingPayload(data);
    } else {
        // WTF?
        qCWarning(LOG_HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT) << tr("Got incoming payload, don't know how to process it!") << payload;
    }
}

}

#include "moc_hemeranetworktransparentoperationclient.cpp"
