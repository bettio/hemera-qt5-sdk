#ifndef HEMERA_INTERNALPLANETINTERFACE_H
#define HEMERA_INTERNALPLANETINTERFACE_H

#include <HemeraCore/AsyncInitDBusObject>

#include <HemeraCore/Planet>

namespace Hemera {

class InternalPlanetInterface : public Hemera::AsyncInitDBusObject
{
    Q_OBJECT
    Q_DISABLE_COPY(InternalPlanetInterface)
    Q_CLASSINFO("D-Bus Interface", "com.ispirata.Hemera.Planet")

    Q_PROPERTY(QStringList activeSatellites READ activeSatellites NOTIFY activeSatellitesChanged)
    Q_PROPERTY(QStringList launchedSatellites READ launchedSatellites NOTIFY launchedSatellitesChanged)

public:
    explicit InternalPlanetInterface(Planet *parent);
    virtual ~InternalPlanetInterface();

    inline QStringList activeSatellites() const { return m_planet->activeSatellites(); }
    inline QStringList launchedSatellites() const { return m_planet->launchedSatellites(); }

public Q_SLOTS:
    void ActivateSatellites(const QStringList &satellites);
    void LaunchSatellite(const QString &satellite);
    void ShutdownSatellite(const QString &satellites);
    void OpenURL(const QString &url, bool tryActivation);

protected Q_SLOTS:
    virtual void initImpl() override final;

Q_SIGNALS:
    void activeSatellitesChanged(const QStringList &activeSatellites);
    void launchedSatellitesChanged(const QStringList &launchedSatellites);

private:
    Planet *m_planet;
};
}

#endif // HEMERA_INTERNALPLANETINTERFACE_H
