#ifndef HEMERA_ROOTOPERATION_P_H
#define HEMERA_ROOTOPERATION_P_H

#include <HemeraCore/RootOperation>

#include <private/HemeraCore/hemeranetworktransparentoperation_p.h>

#include <QtCore/QPointer>

class QLocalSocket;

namespace Hemera {

class RootOperationPrivate : public NetworkTransparentOperationPrivate
{
public:
    RootOperationPrivate(RootOperation* q) : NetworkTransparentOperationPrivate(q) {}

    Q_DECLARE_PUBLIC(RootOperation)

    QPointer< QLocalSocket > client;

    // Q_PRIVATE_SLOTS
    void initServer();
};
}

#endif // HEMERA_NETWORKTRANSPARENTOPERATION_H
