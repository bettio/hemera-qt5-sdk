#ifndef HEMERA_PLANET_H
#define HEMERA_PLANET_H

#include <HemeraCore/AsyncInitDBusObject>

namespace Hemera {

class ServiceInfo;

class PlanetPrivate;
/**
 * @class Planet
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemeraplanet.h <HemeraCore/Planet>
 *
 * @brief Registers the current application as a Planet, allowing satellites management.
 *
 * @note If your application is not a planet, this API is unaccessible and trying to initialize it will
 *       result in a failure. To launch applications or request protocol handlers, refer to
 *       @ref Hemera::ServiceManager.
 *
 * Planet enables an applications belonging to a Planet's Orbit, or a Planetary Application, to manage its
 * satellites. Satellites are Orbits (or Orbital Applications) directly controlled and managed from a Planet. Read
 * @ref PlanetsOrbitsAndSatellites for a detailed explanation of this concept, the understanding of which is required
 * to leverage this mechanism.
 *
 * Planet is flexible and meant to be integrated with your application/context switching logic (if any).
 * Satellites can be either active or inactive, and any number of satellites can be launched.
 *
 * For each Star, only one Planet can be present. Initialization will fail if another Planet already exists.
 *
 * @par Lifetime of Satellites
 *
 * A Satellite's lifetime is usually determined by the Planet, which can shut it down at any time. However,
 * if the Planet shuts down, every Satellite is shut down regardless as well. A Satellite has limited interaction
 * with Gravity and is proxied in 2 levels.
 *
 * Satellites must be explicitly started. If your Planet has the concept of a Session, it should manage this internally
 * by launching Satellites at startup and saving their states when shutting down.
 *
 * @par Activity of Satellites
 *
 * Satellites are applications, and as such they follow the concepts of Hemera's state machine for Applications. An
 * application can be either started or stopped when alive, and the Activity determined by its Star makes it work.
 *
 * Planets can decide arbitrarly the behavioral policy for Satellite activation. By default, everything needs to be
 * explicitly set, but a Planet can be configured to perform some automated activation task. It can be easily configured to
 * keep every Satellite active at all times, matching the behavior of a Desktop Shell, or emulate a commercial Mobile shell.
 *
 * If your policy isn't fully automated, you always need to explicitly
 * activate or deactivate Satellites, depending on your context. Of course, there could be no active satellites.
 * If you are operating with @ref KeepAtMostOneActive, no explicit deactivation is required, unless you want to deactivate
 * all satellites (e.g.:: return to the shell): request for activation of a new satellite will implicitly trigger the deactivation of the
 * active one.
 *
 * @sa Application
 */
class Planet : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Planet)
    Q_DECLARE_PRIVATE_D(d_h_ptr, Planet)

    Q_PROPERTY(QStringList activeSatellites READ activeSatellites NOTIFY activeSatellitesChanged)
    Q_PROPERTY(QStringList launchedSatellites READ launchedSatellites NOTIFY launchedSatellitesChanged)

public:
    /**
     * This enum represents the activation policy the Application Handler will keep when Planet deals with
     * the Satellite.
     */
    enum class ActivationPolicy {
        /**
         * This policy implies no action will be automatically taken by Parsec. Every activation
         * and deactivation request will need to be explicit. No other policies can be set.
         */
        FullyExplicit = 0,
        /// When a satellite gets launched, it also gets activated immediately.
        ActivateOnLaunch = 1 << 0,
        /**
         * When a satellite gets activated, all the others are deactivated automatically.
         * Requesting a multiple satellite activation will fail.
         */
        KeepAtMostOneActive = 1 << 1,


        // ALIASES
        /**
         * Shortcut for creating a policy set which matches a typical mobile shell.
         * It is the same as ActivateOnLaunch | KeepAtMostOneActive | PlanetIsAlwaysActive.
         */
        MobileLikeShell = ActivateOnLaunch | KeepAtMostOneActive
    };
    Q_DECLARE_FLAGS(ActivationPolicies, ActivationPolicy)

    explicit Planet(QObject* parent);
    virtual ~Planet();

    QStringList launchedSatellites() const;
    QStringList activeSatellites() const;

    ActivationPolicies activationPolicies() const;

protected Q_SLOTS:
    /**
     * Sets the activation policies for this Planet. This method has to be called during the Planet's
     * initialization phase and cannot be called afterwards.
     *
     * This means the activation policies cannot be changed during the Planet's execution. If you need
     * a more flexible policy, you can set FullyExplicit (which is also default) to explicitly set
     * activation for each satellite (and the planet itself).
     *
     * @p policies The policies for this planet.
     */
    void setActivationPolicies(ActivationPolicies policies);

    Operation *launchSatellite(const QString &orbitName);
    Operation *launchSatellite(const Hemera::ServiceInfo &service);
    Operation *shutdownSatellite(const QString &satellite);
    Operation *shutdownAllSatellites();

    Operation *activateSatellites(const QStringList &satellites);
    Operation *deactivateSatellites(const QStringList &satellites);
    Operation *deactivateAllSatellites();

    virtual Operation *handleActivationRequest(const QString &from, const QStringList &activation) = 0;
    virtual Operation *handleOpenURLRequest(const QString &from, const QUrl &url, bool tryActivation) = 0;
    virtual Operation *handleLaunchRequest(const QString &from, const QString &serviceId) = 0;
    virtual Operation *handleShutdownRequest(const QString &from, const QString &serviceId) = 0;

Q_SIGNALS:
    void activeSatellitesChanged(const QStringList &activeSatellites);
    void launchedSatellitesChanged(const QStringList &launchedSatellites);

private:
    friend class InternalPlanetInterface;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Hemera::Planet::ActivationPolicies)

#endif // HEMERA_PLANET_H
