/*
 *
 */

#include "hemeradbusapplicationholder_p.h"

#include "dbusholderadaptor.h"

#include <HemeraCore/Literals>

namespace Hemera
{

DBusApplicationHolder::DBusApplicationHolder(const QString &applicationId, QObject *parent)
    : Hemera::AsyncInitObject(parent)
    , m_service(applicationId + QStringLiteral(".Holder"))
{
}

DBusApplicationHolder::~DBusApplicationHolder()
{
}

void DBusApplicationHolder::initImpl()
{
    // Do the service registration dance. Register service as the last thing we do.
    if (!QDBusConnection::sessionBus().registerObject(Literals::literal(Literals::DBus::dBusHolderPath()), this)) {
        setInitError(Literals::literal(Literals::Errors::registerObjectFailed()),
                     tr("Couldn't register the bus holder object. This is a severe runtime error, your Orbit might be compromised."));
        return;
    }

    new DBusHolderAdaptor(this);

    if (!QDBusConnection::sessionBus().registerService(m_service)) {
        setInitError(Literals::literal(Literals::Errors::registerServiceFailed()),
                     tr("Couldn't register the bus holder service. This means a problem in the installation of the app."));
        return;
    }

    setReady();
}

void DBusApplicationHolder::Release()
{
    // Release service
    QDBusConnection::sessionBus().unregisterService(m_service);
    // Notify, so that Application can release the lock
    Q_EMIT released();
    // Self-destroy
    QTimer::singleShot(500, this, SLOT(deleteLater()));
}

}
