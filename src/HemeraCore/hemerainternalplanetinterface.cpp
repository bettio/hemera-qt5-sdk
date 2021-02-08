#include "hemerainternalplanetinterface_p.h"

#include <HemeraCore/Literals>
#include <HemeraCore/Operation>

#include "planetadaptor.h"

namespace Hemera
{

InternalPlanetInterface::InternalPlanetInterface(Planet *planet)
    : AsyncInitDBusObject(planet)
    , m_planet(planet)
{
}

InternalPlanetInterface::~InternalPlanetInterface()
{
}

void InternalPlanetInterface::initImpl()
{
    // Relay signals
    connect(m_planet, &Hemera::Planet::activeSatellitesChanged, this, &Hemera::InternalPlanetInterface::activeSatellitesChanged);
    connect(m_planet, &Hemera::Planet::launchedSatellitesChanged, this, &Hemera::InternalPlanetInterface::launchedSatellitesChanged);

    if (!QDBusConnection(Literals::DBus::starBus()).registerService(Literals::literal(Literals::DBus::planetService()))) {
        setInitError(Literals::literal(Literals::Errors::registerServiceFailed()),
                     tr("Couldn't register the bus service. A planet is likely already active, or you can't register a Planet."));
        return;
    }

    if (!QDBusConnection(Literals::DBus::starBus()).registerObject(Literals::literal(Literals::DBus::planetPath()), this)) {
        setInitError(Literals::literal(Literals::Errors::registerObjectFailed()),
                     tr("Couldn't register the bus object. This is a severe runtime error, your Orbit might be compromised."));
        return;
    }
    new PlanetAdaptor(this);

    setReady();
}

void InternalPlanetInterface::ActivateSatellites(const QStringList &satellites)
{
    if (!calledFromDBus()) {
        return;
    }

    setDelayedReply(true);
    QDBusMessage requestMessage = message();
    connect(m_planet->handleActivationRequest(message().service(), satellites), &Operation::finished, [this, requestMessage] (Operation *op) {
        if (op->isError()) {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createErrorReply(op->errorName(), op->errorMessage()));
        } else {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createReply());
        }
    });
}

void InternalPlanetInterface::LaunchSatellite(const QString &satellite)
{
    if (!calledFromDBus()) {
        return;
    }

    setDelayedReply(true);
    QDBusMessage requestMessage = message();
    connect(m_planet->handleLaunchRequest(message().service(), satellite), &Operation::finished, [this, requestMessage] (Operation *op) {
        if (op->isError()) {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createErrorReply(op->errorName(), op->errorMessage()));
        } else {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createReply());
        }
    });
}

void InternalPlanetInterface::ShutdownSatellite(const QString &satellite)
{
    if (!calledFromDBus()) {
        return;
    }

    setDelayedReply(true);
    QDBusMessage requestMessage = message();
    connect(m_planet->handleShutdownRequest(message().service(), satellite), &Operation::finished, [this, requestMessage] (Operation *op) {
        if (op->isError()) {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createErrorReply(op->errorName(), op->errorMessage()));
        } else {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createReply());
        }
    });
}

void InternalPlanetInterface::OpenURL(const QString &url, bool tryActivation)
{
    if (!calledFromDBus()) {
        return;
    }

    setDelayedReply(true);
    QDBusMessage requestMessage = message();
    connect(m_planet->handleOpenURLRequest(message().service(), url, tryActivation), &Operation::finished, [this, requestMessage] (Operation *op) {
        if (op->isError()) {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createErrorReply(op->errorName(), op->errorMessage()));
        } else {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createReply());
        }
    });
}

}
