#ifndef HEMERA_NETWORKTRANSPARENTOPERATION_P_H
#define HEMERA_NETWORKTRANSPARENTOPERATION_P_H

#include <HemeraCore/NetworkTransparentOperation>

#include <private/HemeraCore/hemeraoperationwithprogress_p.h>

#include <QtCore/QJsonObject>

namespace Hemera {

class NetworkTransparentOperationPrivate : public OperationWithProgressPrivate
{
public:
    NetworkTransparentOperationPrivate(NetworkTransparentOperation* q) : OperationWithProgressPrivate(q) {}

    Q_DECLARE_PUBLIC(NetworkTransparentOperation)

    QString id;
    QString instanceId;
    QJsonObject parameters;
    QJsonObject returnValue;

    // Q_PRIVATE_SLOTS
    void sendStatusUpdate();
};
}

#endif // HEMERA_NETWORKTRANSPARENTOPERATION_H
