#include "hemeradbustest.h"

#include <cstdlib>

#include <QtCore/QTimer>

#include <QtDBus/QDBusConnection>

Q_LOGGING_CATEGORY(LOG_HEMERA_DBUSTEST, "Hemera::DBusTest")

namespace Hemera {

namespace Test {

DBusTest::DBusTest(QObject *parent)
    : Test(parent)
{
}

DBusTest::~DBusTest()
{
}

void DBusTest::initTestCaseImpl()
{
    QVERIFY(QDBusConnection::systemBus().isConnected());
}

void DBusTest::expectSuccessfulCall(QDBusPendingCallWatcher *watcher)
{
    if (watcher->isError()) {
        qCWarning(LOG_HEMERA_DBUSTEST).nospace() << watcher->error().name()
            << ": " << watcher->error().message();
        mLoop->exit(1);
        return;
    }

    mLoop->exit(0);
}

}

}
