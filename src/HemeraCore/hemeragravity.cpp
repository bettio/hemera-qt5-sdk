#include "hemeragravity_p.h"

#include "hemeraprivateoperations_p.h"

#include <QtCore/QLoggingCategory>

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

#include "dbusobjectinterface.h"
#include "parsecinterface.h"

Q_LOGGING_CATEGORY(LOG_HEMERA_GRAVITY, "Hemera::Gravity")

namespace Hemera
{

// OPERATIONS

Hemera::Operation *Gravity::Private::requestOrbitSwitchInhibition(const QString &reason)
{
    return new DBusVoidOperation(parsec->inhibitOrbitSwitch(reason));
}

Hemera::Operation *Gravity::Private::releaseOrbitSwitchInhibition()
{
    return new DBusVoidOperation(parsec->releaseOrbitSwitchInhibition());
}

QString Hemera::Gravity::star()
{
    return QLatin1String(qgetenv("HEMERA_STAR"));
}


Gravity::Gravity(QObject *parent)
    : AsyncInitObject(parent)
    , d(new Private)
{
}

Gravity::~Gravity()
{
    delete d;
}

Operation *Gravity::requestOrbitSwitch(const QString& orbit)
{
    if (isSwitchingOrbit()) {
        return nullptr;
    }

    return new DBusVoidOperation(d->parsec->requestOrbitSwitch(orbit), this);
}

bool Gravity::isDeveloperModeActive() const
{
    return d->deviceInDeveloperMode;
}

bool Gravity::isSwitchingOrbit() const
{
    return d->activeOrbit == QStringLiteral("switchingOrbit");
}

bool Gravity::isSwitchingInhibited() const
{
    return d->isInhibited;
}

void Gravity::initImpl()
{
    // Connect to Parsec
    d->parsec = new com::ispirata::Hemera::Parsec(Literals::literal(Literals::DBus::parsecService()),
                                                  Literals::literal(Literals::DBus::parsecPath()),
                                                  QDBusConnection(Literals::DBus::starBus()), this);

    if (!d->parsec->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus Manager is not available. The daemon is probably not running."));
        return;
    }

    d->parsecDBusObject = new com::ispirata::Hemera::DBusObject(Literals::literal(Literals::DBus::parsecService()),
                                                                Literals::literal(Literals::DBus::parsecPath()),
                                                                QDBusConnection(Literals::DBus::starBus()), this);

    if (!d->parsecDBusObject->isValid()) {
        setInitError(Literals::literal(Literals::Errors::dbusObjectNotAvailable()),
                     tr("The remote DBus object is not available. The manager daemon is probably not running."));
        return;
    }

    // Are we on a development board?
    // We are forced to be synchronous by QDBusAbstractInterface shitty APIs.
    QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered(Literals::literal(Literals::DBus::developerModeService()));
    if (reply.isValid()) {
        d->deviceInDeveloperMode = reply.value();
    } else {
        qCWarning(LOG_HEMERA_GRAVITY) << "Querying Gravity for developer mode failed. Assuming production as the safest operational mode.";
        d->deviceInDeveloperMode = false;
    }

    setParts(2);

    // Cache properties and connect to signals
    DBusVariantMapOperation *operation = new DBusVariantMapOperation(d->parsecDBusObject->allProperties(), this);
    connect(operation, &Operation::finished, [this, operation] {
        if (operation->isError()) {
            setInitError(operation->errorName(), operation->errorMessage());
            return;
        }

        // Set the various properties
        QVariantMap result = operation->result();
        d->name = result.value(QStringLiteral("name")).toString();
        d->baseConfigPath = result.value(QStringLiteral("baseConfigPath")).toString();

        d->activeOrbit = result.value(QStringLiteral("activeOrbit")).toString();
        d->inhibitionReasons = result.value(QStringLiteral("inhibitionReasons")).toMap();
        d->isInhibited = result.value(QStringLiteral("isOrbitSwitchInhibited")).toBool();

        setOnePartIsReady();
    });

    connect(d->parsecDBusObject, &com::ispirata::Hemera::DBusObject::propertiesChanged, [this] (const QVariantMap &changed) {
        if (changed.contains(QStringLiteral("activeOrbit"))) {
            d->activeOrbit = changed.value(QStringLiteral("activeOrbit")).toString();
            Q_EMIT activeOrbitChanged(d->activeOrbit);
        }
        if (changed.contains(QStringLiteral("inhibitionReasons"))) {
            d->inhibitionReasons = changed.value(QStringLiteral("inhibitionReasons")).toMap();
        }
        if (changed.contains(QStringLiteral("isOrbitSwitchInhibited"))) {
            d->isInhibited = changed.value(QStringLiteral("isOrbitSwitchInhibited")).toBool();
        }
    });

    setOnePartIsReady();
}

Gravity::OrbitType Gravity::orbitType()
{
    return OrbitType::Standalone;
}

Operation* Gravity::requestOpenURL(const QUrl& url, bool tryActivation)
{
    return new DBusVoidOperation(d->parsec->openURL(url.toString(), tryActivation));
}

QString Gravity::activeOrbit() const
{
    return d->activeOrbit;
}

QString Gravity::currentOrbit() const
{
    return d->name;
}

}
