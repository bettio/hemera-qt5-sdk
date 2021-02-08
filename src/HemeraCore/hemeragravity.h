#ifndef HEMERA_HEMERAGRAVITY_H
#define HEMERA_HEMERAGRAVITY_H

#include <HemeraCore/AsyncInitObject>
#include <HemeraCore/Global>

namespace Hemera {

class Operation;

class HEMERA_QT5_SDK_EXPORT Gravity : public AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Gravity)

    Q_PROPERTY (QString activeOrbit READ activeOrbit NOTIFY activeOrbitChanged)

public:
    enum class OrbitType {
        Unknown = HEMERA_ENUM_Gravity_OrbitType_Unknown,
        Standalone = HEMERA_ENUM_Gravity_OrbitType_Standalone,
        Planet = HEMERA_ENUM_Gravity_OrbitType_Planet,
        Satellite = HEMERA_ENUM_Gravity_OrbitType_Satellite
    };

    explicit Gravity(QObject *parent = nullptr);
    virtual ~Gravity();

    static QString star();

    QString currentOrbit() const;
    OrbitType orbitType();

    bool isDeveloperModeActive() const;
    bool isSwitchingOrbit() const;
    bool isSwitchingInhibited() const;

    QString activeOrbit() const;

public Q_SLOTS:
    Hemera::Operation *requestOrbitSwitch(const QString &orbit);
    Hemera::Operation *requestOpenURL(const QUrl &url, bool tryActivation = true);

protected Q_SLOTS:
    virtual void initImpl() override final;

Q_SIGNALS:
    void activeOrbitChanged(const QString &orbit);

private:
    class Private;
    Private * const d;

    friend class Application;
    friend class ApplicationPrivate;
};

}

#endif // HEMERA_HEMERAGRAVITY_H
