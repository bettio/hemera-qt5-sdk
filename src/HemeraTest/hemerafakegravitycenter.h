#ifndef HEMERA_TESTS_FAKEGRAVITYCENTER_H
#define HEMERA_TESTS_FAKEGRAVITYCENTER_H

#include <HemeraCore/AsyncInitDBusObject>

namespace Hemera {

namespace Test {

class FakeGravityCenter : public Hemera::AsyncInitDBusObject
{
    Q_OBJECT

    Q_PROPERTY(QString activeOrbit            READ activeOrbit              NOTIFY activeOrbitChanged)
    Q_PROPERTY(bool isOrbitSwitchInhibited    READ isOrbitSwitchInhibited   NOTIFY inhibitionReasonsChanged)
    Q_PROPERTY(QVariantMap inhibitionReasons    READ inhibitionReasons          NOTIFY inhibitionReasonsChanged)

public:
    explicit FakeGravityCenter(QObject *parent = 0);
    virtual ~FakeGravityCenter();

    QString activeOrbit() const;
    uint inhibitOrbitSwitch(const QString &in0, const QString &in1);
    QVariantMap inhibitionReasons();
    bool isOrbitSwitchInhibited() const;
    QDBusVariant orbitProperty(const QString &in0, const QString &in1) const;
    void releaseOrbitSwitchInhibition(uint in0);
    void requestOrbitSwitch(const QString &orbit);

protected Q_SLOTS:
    void initImpl() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void activeOrbitChanged();
    void inhibitionReasonsChanged();

private:
    class Private;
    Private * const d;
};

}

}

#endif // FAKEGRAVITYCENTER_H
