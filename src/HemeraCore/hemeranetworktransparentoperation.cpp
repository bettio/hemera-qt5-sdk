#include "hemeranetworktransparentoperation_p.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <HemeraCore/Literals>

Q_LOGGING_CATEGORY(LOG_HEMERA_NETWORKTRANSPARENTOPERATION, "Hemera::NetworkTransparentOperation")

namespace Hemera {

void NetworkTransparentOperationPrivate::sendStatusUpdate()
{
    Q_Q(NetworkTransparentOperation);

    if (instanceId.isEmpty()) {
        qCWarning(LOG_HEMERA_NETWORKTRANSPARENTOPERATION)
            << NetworkTransparentOperation::tr("Trying to send a status update before an instance ID has been set! "
                                               "This is likely a bug in the server implementation.");
    }

    QJsonObject data;
    data.insert(QStringLiteral("type"), QStringLiteral("NetworkTransparentOperationStatus"));
    data.insert(QStringLiteral("id"), id);
    data.insert(QStringLiteral("instanceId"), instanceId);

    QJsonObject j;
    j.insert(QStringLiteral("progress"), q->progress());
    j.insert(QStringLiteral("statusMessage"), q->statusMessage());
    j.insert(QStringLiteral("started"), q->isStarted());
    j.insert(QStringLiteral("finished"), q->isFinished());
    j.insert(QStringLiteral("valid"), q->isValid());
    j.insert(QStringLiteral("error"), q->isError());
    if (q->isError()) {
        j.insert(QStringLiteral("errorName"), q->errorName());
        j.insert(QStringLiteral("errorMessage"), q->errorMessage());
    }
    if (!returnValue.isEmpty()) {
        j.insert(QStringLiteral("returnValue"), returnValue);
    }

    data.insert(QStringLiteral("status"), j);

    if (q->isFinished()) {
        // If we're done, time to disconnect from everything to avoid spurious updates.
        QObject::disconnect(q, 0, q, SLOT(sendStatusUpdate()));
    }

    Q_EMIT q->newOutgoingPayload(data);
}



NetworkTransparentOperation::NetworkTransparentOperation(const QString &id, QObject* parent)
    : NetworkTransparentOperation(*new NetworkTransparentOperationPrivate(this), id, parent)
{
}

NetworkTransparentOperation::NetworkTransparentOperation(NetworkTransparentOperationPrivate& dd, const QString &id, QObject* parent)
    // We always request an explicit start: the client needs to perform the request.
    : OperationWithProgress(dd, ExplicitStartOption, parent)
{
    Q_D(NetworkTransparentOperation);

    d->id = id;

    // If we're ok, it's time to connect some signals here and there.
    connect(this, SIGNAL(started(Hemera::Operation*)), this, SLOT(sendStatusUpdate()));
    connect(this, SIGNAL(finished(Hemera::Operation*)), this, SLOT(sendStatusUpdate()));
    connect(this, SIGNAL(progressChanged(int)), this, SLOT(sendStatusUpdate()));
    connect(this, SIGNAL(statusMessageChanged(QString)), this, SLOT(sendStatusUpdate()));
}

NetworkTransparentOperation::~NetworkTransparentOperation()
{
}

QString NetworkTransparentOperation::id() const
{
    Q_D(const NetworkTransparentOperation);
    return d->id;
}

QString NetworkTransparentOperation::instanceId() const
{
    Q_D(const NetworkTransparentOperation);
    return d->instanceId;
}

void NetworkTransparentOperation::setInstanceId(const QString &instanceId)
{
    Q_D(NetworkTransparentOperation);
    d->instanceId = instanceId;
}

QJsonObject NetworkTransparentOperation::parameters() const
{
    Q_D(const NetworkTransparentOperation);
    return d->parameters;
}

void NetworkTransparentOperation::setParameters(const QJsonObject &parameters)
{
    Q_D(NetworkTransparentOperation);
    d->parameters = parameters;
}

void NetworkTransparentOperation::setReturnValue(const QJsonObject &returnValue)
{
    Q_D(NetworkTransparentOperation);
    d->returnValue = returnValue;
}

void NetworkTransparentOperation::processIncomingPayload(const QJsonObject& payload)
{
    // We shall just accept the "start" command.
    if (payload.value(QStringLiteral("command")).toString() == QStringLiteral("start")) {
        if (!isStarted()) {
            // Start
            qCDebug(LOG_HEMERA_NETWORKTRANSPARENTOPERATION) << tr("Starting operation after request from client...");
            start();
        }
    }
}

}

#include "moc_hemeranetworktransparentoperation.cpp"
