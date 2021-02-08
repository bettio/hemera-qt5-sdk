#ifndef HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT_P_H
#define HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT_P_H

#include <HemeraCore/NetworkTransparentOperationClient>
#include <private/HemeraCore/hemeraoperationwithprogress_p.h>

namespace Hemera {

class NetworkTransparentOperationClientPrivate : public Hemera::OperationWithProgressPrivate
{
public:
    explicit NetworkTransparentOperationClientPrivate(NetworkTransparentOperationClient *q)
        : OperationWithProgressPrivate(q, PrivateExecutionOption::SetStartedExplicitly), connected(false)
        , transportReady(false) {}

    Q_DECLARE_PUBLIC(NetworkTransparentOperationClient)

    QJsonObject parameters;
    QJsonObject returnValue;
    bool connected;
    bool transportReady;
    int requestToken;
    QString remoteOperationId;
    QString instanceId;

    QString transportErrorName;
    QString transportErrorMessage;
};

}

#endif // HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT_P_H
