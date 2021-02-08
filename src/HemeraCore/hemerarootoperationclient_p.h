#ifndef HEMERA_ROOTOPERATIONCLIENT_P_H
#define HEMERA_ROOTOPERATIONCLIENT_P_H

#include <HemeraCore/RootOperationClient>
#include <HemeraCore/hemeranetworktransparentoperationclient_p.h>

#include <QtCore/QDebug>
#include <QtCore/QPointer>

class QLocalSocket;

namespace Hemera {

class RootOperationClientPrivate : public NetworkTransparentOperationClientPrivate
{
public:
    explicit RootOperationClientPrivate(RootOperationClient *q)
        : NetworkTransparentOperationClientPrivate(q) {}

    Q_DECLARE_PUBLIC(RootOperationClient)

    QPointer< QLocalSocket > socket;

    // Q_PRIVATE_SLOTS
    void initClient();
};

}

#endif // HEMERA_ROOTOPERATIONCLIENT_P_H
