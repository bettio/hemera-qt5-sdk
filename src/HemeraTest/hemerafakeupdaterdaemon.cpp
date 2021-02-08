#include "hemerafakesoftwareManagerdaemon.h"
#include "softwareManagerdaemonadaptor.h"

#include <HemeraCore/Literals>

#include <QtTest/QTest>
#include <QtDBus/QDBusConnection>

namespace Hemera {

namespace Test {

class FakeSoftwareManagerDaemon::Private
{
public:
    Private() {};

    QStringList availableUpdates;
};

FakeSoftwareManagerDaemon::FakeSoftwareManagerDaemon(QObject *parent)
    : AsyncInitDBusObject(parent)
    , d(new Private)
{
}


FakeSoftwareManagerDaemon::~FakeSoftwareManagerDaemon()
{
}


QStringList FakeSoftwareManagerDaemon::availableUpdates()
{
    return d->availableUpdates;
}


void FakeSoftwareManagerDaemon::initImpl()
{
    QVERIFY(QDBusConnection::systemBus().registerService(Hemera::Literals::literal(Hemera::Literals::DBus::softwareManagerDaemonService())));
    QVERIFY(QDBusConnection::systemBus().registerObject(Hemera::Literals::literal(Hemera::Literals::DBus::softwareManagerDaemonPath()), this));

    new SoftwareManagerDaemonAdaptor(this);

    setReady();
}


void FakeSoftwareManagerDaemon::triggerAvailableUpdates()
{
    d->availableUpdates << QStringLiteral("update1") << QStringLiteral("update2") << QStringLiteral("update3");
    Q_EMIT availableUpdatesChanged(d->availableUpdates);
}

}

}
