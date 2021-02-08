#include "hemeraplanet.h"

#include "hemeraasyncinitobject_p.h"
#include "hemerainternalplanetinterface_p.h"

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Gravity>
#include <HemeraCore/Literals>
#include <HemeraCore/ServiceManager>

#include "dbusobjectinterface.h"
#include "applicationhandlerinterface.h"

namespace Hemera
{

class PlanetPrivate : public Hemera::AsyncInitObjectPrivate
{
public:
    PlanetPrivate(AsyncInitObject* q) : AsyncInitObjectPrivate(q) , activationPolicies(Planet::ActivationPolicy::FullyExplicit) {}
    virtual ~PlanetPrivate() {}

    Q_DECLARE_PUBLIC(Planet)

    virtual void initHook() override final;

    Planet::ActivationPolicies activationPolicies;

    InternalPlanetInterface *interface;
    com::ispirata::Hemera::Parsec::ApplicationHandler *applicationHandlerInterface;
    com::ispirata::Hemera::DBusObject *applicationHandlerDBusObject;

    QStringList activeSatellites;
    QStringList launchedSatellites;
};

void PlanetPrivate::initHook()
{
    // Bring up the interface
    Q_Q(Planet);
    interface = new InternalPlanetInterface(q);

    QObject::connect(interface->init(), &Hemera::Operation::finished, [this, q] (Hemera::Operation *op) {
        if (op->isError()) {
            q->setInitError(op->errorName(), op->errorMessage());
            return;
        }

        // Create connection to the Application Handler
        applicationHandlerInterface = new com::ispirata::Hemera::Parsec::ApplicationHandler(Literals::literal(Literals::DBus::parsecService()),
                                                                                            Literals::literal(Literals::DBus::applicationHandlerPath()),
                                                                                            QDBusConnection(Literals::DBus::starBus()), q);

        if (!applicationHandlerInterface->isValid()) {
            q->setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                            Planet::tr("The remote DBus Manager is not available. The daemon is probably not running."));
            return;
        }

        applicationHandlerDBusObject = new com::ispirata::Hemera::DBusObject(Literals::literal(Literals::DBus::parsecService()),
                                                                             QString::fromLatin1(Literals::DBus::applicationHandlerPath()).arg(Gravity::star()),
                                                                             QDBusConnection(Literals::DBus::starBus()), q);

        if (!applicationHandlerDBusObject->isValid()) {
            q->setInitError(Literals::literal(Literals::Errors::dbusObjectNotAvailable()),
                            Planet::tr("The remote DBus object is not available. The manager daemon is probably not running."));
            return;
        }

        // Cache properties and connect to signals
        DBusVariantMapOperation *operation = new DBusVariantMapOperation(applicationHandlerDBusObject->allProperties(), q);
        QObject::connect(operation, &Operation::finished, [this, q, operation] {
            if (operation->isError()) {
                q->setInitError(operation->errorName(), operation->errorMessage());
                return;
            }

            // Set the various properties
            QVariantMap result = operation->result();
            launchedSatellites = result.value(QStringLiteral("LaunchedSatellites")).toStringList();
            activeSatellites = result.value(QStringLiteral("ActiveSatellites")).toStringList();
            activationPolicies = static_cast<Planet::ActivationPolicies>(result.value(QStringLiteral("ActivationPolicies")).toUInt());

            Hemera::AsyncInitObjectPrivate::initHook();
        });

        QObject::connect(applicationHandlerDBusObject, &com::ispirata::Hemera::DBusObject::propertiesChanged, [this, q] (const QVariantMap &changed) {
            if (changed.contains(QStringLiteral("ActiveSatellites"))) {
                activeSatellites = changed.value(QStringLiteral("ActiveSatellites")).toStringList();
                Q_EMIT q->activeSatellitesChanged(activeSatellites);
            }
            if (changed.contains(QStringLiteral("LaunchedSatellites"))) {
                launchedSatellites = changed.value(QStringLiteral("LaunchedSatellites")).toStringList();
                Q_EMIT q->launchedSatellitesChanged(launchedSatellites);
            }
            if (changed.contains(QStringLiteral("ActivationPolicies"))) {
                activationPolicies = static_cast<Planet::ActivationPolicies>(changed.value(QStringLiteral("ActivationPolicies")).toUInt());
            }
        });
    });
}

Planet::Planet(QObject* parent)
    : AsyncInitObject(*new PlanetPrivate(this), parent)
{
}

Planet::~Planet()
{
}

Planet::ActivationPolicies Planet::activationPolicies() const
{
    Q_D(const Planet);
    return d->activationPolicies;
}

void Planet::setActivationPolicies(ActivationPolicies policies)
{
    if (isReady()) {
        // We can't do this.
        return;
    }

    Q_D(const Planet);
    d->applicationHandlerInterface->SetActivationPolicies(static_cast<uint>(policies));
}

QStringList Planet::activeSatellites() const
{
    Q_D(const Planet);
    return d->activeSatellites;
}

QStringList Planet::launchedSatellites() const
{
    Q_D(const Planet);
    return d->launchedSatellites;
}

Operation* Planet::deactivateAllSatellites()
{
    Q_D(const Planet);
    return new DBusVoidOperation(d->applicationHandlerInterface->DeactivateAllSatellites());
}

Operation* Planet::activateSatellites(const QStringList& satellites)
{
    Q_D(const Planet);
    return new DBusVoidOperation(d->applicationHandlerInterface->ActivateSatellites(satellites));
}

Operation* Planet::deactivateSatellites(const QStringList& satellites)
{
    Q_D(const Planet);
    return new DBusVoidOperation(d->applicationHandlerInterface->DeactivateSatellites(satellites));
}

Operation* Planet::launchSatellite(const QString& orbitName)
{
    Q_D(const Planet);
    return new DBusVoidOperation(d->applicationHandlerInterface->LaunchOrbitAsSatellite(orbitName));
}

Operation* Planet::launchSatellite(const ServiceInfo &service)
{
    if (service.type() != ServiceInfo::ServiceType::Orbit) {
        return new FailureOperation(Literals::literal(Literals::Errors::badRequest()), tr("This service is not an Orbit"));
    }

    return launchSatellite(service.id());
}

Operation* Planet::shutdownSatellite(const QString& orbitName)
{
    Q_D(const Planet);
    return new DBusVoidOperation(d->applicationHandlerInterface->ShutdownSatellite(orbitName));
}

Operation* Planet::shutdownAllSatellites()
{
    Q_D(const Planet);
    return new DBusVoidOperation(d->applicationHandlerInterface->ShutdownAllSatellites());
}

}

#include "moc_hemeraplanet.cpp"
