#include <HemeraTest/FakeHeadlessApplication>
#include <HemeraTest/FakeGravityCenter>
#include <HemeraTest/FakeOrbitManager>
#include <HemeraTest/TestWithApplicationEnvironment>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/Literals>
#include <HemeraCore/Gravity>
#include <HemeraCore/Operation>

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>

#include <QtTest/QSignalSpy>
#include <QtTest/QTest>


using namespace Hemera;


class HeadlessApplicationBasics : public Hemera::Test::TestWithApplicationEnvironment
{
    Q_OBJECT

public:
    HeadlessApplicationBasics(QObject *parent = nullptr)
        : TestWithApplicationEnvironment(parent)
    {}

private Q_SLOTS:
    void initTestCase();        // inits the necessaary for test to execute
    void init();

    void testStartApplication();
    void testInhibition();
    void testStopApplication();
    void testRollback();
    void testQuitApplication();
    void testStartFatals();
    void testStopFatals();

    void cleanupTestCase();

private:
    Hemera::Test::FakeHeadlessApplication *m_fakeApplication;
    QThread *m_thread;
};

void HeadlessApplicationBasics::initTestCase()
{
    QVERIFY(!Application::instance());

    // Test init failure
    Hemera::Application *app = new Hemera::Test::FakeHeadlessApplication;
    QCOMPARE(Application::instance(), app);
    QSignalSpy initFailureSignals(app, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));
    Hemera::Operation *op = app->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);
    while (initFailureSignals.count() < 4) {
        QVERIFY(initFailureSignals.wait(1000));
    }
    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::ReadyForShutdown);
    op = app->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QSignalSpy destroyedSignal(app, SIGNAL(destroyed()));
    app->deleteLater();
    QVERIFY(destroyedSignal.wait(1000));

    TestWithApplicationEnvironment::initTestCaseImpl();

    m_fakeApplication = new Hemera::Test::FakeHeadlessApplication;
    QSignalSpy applicationStatus(m_fakeApplication, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));

    m_fakeApplication->init();

    QVERIFY(applicationStatus.wait());
    QVERIFY(fakeGravityCenter()->isReady());
    QVERIFY(fakeOrbitManager()->isReady());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Initializing);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);

    QVERIFY(QDBusConnection::sessionBus().interface()->registeredServiceNames().value().contains(QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
}

void HeadlessApplicationBasics::init()
{
    m_fakeApplication->setShouldFail(false);
}

void HeadlessApplicationBasics::testStartApplication()
{
    QSignalSpy applicationStatus(m_fakeApplication, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));

    // Make it fail
    QMetaObject::invokeMethod(fakeOrbitManager(), "stopApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);
    // We shouldn't be able to call this from outside the bus
    m_fakeApplication->start();
    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);
    QMetaObject::invokeMethod(fakeOrbitManager(), "startApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Starting);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Running);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Running);
}

void HeadlessApplicationBasics::testInhibition()
{
    QVERIFY(!m_fakeApplication->releaseOrbitSwitchInhibition());
    QVERIFY(!m_fakeApplication->publicGravity()->isSwitchingInhibited());

    Hemera::Operation *op = m_fakeApplication->requestOrbitSwitchInhibition(QStringLiteral("fail"));
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(!m_fakeApplication->publicGravity()->isSwitchingInhibited());

    op = m_fakeApplication->requestOrbitSwitchInhibition();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)), Qt::QueuedConnection);
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(!m_fakeApplication->requestOrbitSwitchInhibition());
    QVERIFY(m_fakeApplication->publicGravity()->isSwitchingInhibited());

    op = m_fakeApplication->releaseOrbitSwitchInhibition();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(!m_fakeApplication->publicGravity()->isSwitchingInhibited());
}

void HeadlessApplicationBasics::testStopApplication()
{
    QSignalSpy applicationStatus(m_fakeApplication, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));

    // Make it fail
    QMetaObject::invokeMethod(fakeOrbitManager(), "startApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Running);
    // We shouldn't be able to call this from outside the bus
    m_fakeApplication->stop();
    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Running);
    QMetaObject::invokeMethod(fakeOrbitManager(), "stopApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopping);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);
}

void HeadlessApplicationBasics::testRollback()
{
    m_fakeApplication->setShouldFail(true);

    QSignalSpy applicationStatus(m_fakeApplication, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));

    QMetaObject::invokeMethod(fakeOrbitManager(), "startApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Starting);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);

    QMetaObject::invokeMethod(fakeOrbitManager(), "quitApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ShuttingDown);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);

    m_fakeApplication->setShouldFail(false);

    QMetaObject::invokeMethod(fakeOrbitManager(), "startApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Starting);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Running);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Running);

    m_fakeApplication->setShouldFail(true);

    QMetaObject::invokeMethod(fakeOrbitManager(), "stopApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopping);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Running);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Running);
}

void HeadlessApplicationBasics::testQuitApplication()
{
    QSignalSpy applicationStatus(m_fakeApplication, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::Running);

    // Quit while started
    QMetaObject::invokeMethod(fakeOrbitManager(), "quitApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    // They come one after the other due to the fact stop is triggered manually into the event loop
    QCOMPARE(applicationStatus.count(), 3);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopping);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ShuttingDown);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QMetaObject::invokeMethod(fakeOrbitManager(), "quitApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    // Should fail
    QCOMPARE(m_fakeApplication->applicationStatus(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QSignalSpy destroyedSignal(m_fakeApplication, SIGNAL(destroyed()));
    m_fakeApplication->deleteLater();
    QVERIFY(destroyedSignal.wait(1000));
}

void HeadlessApplicationBasics::testStartFatals()
{
    Hemera::Test::FakeHeadlessApplication *app = new Hemera::Test::FakeHeadlessApplication;
    QSignalSpy applicationStatus(app, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));
    app->init();
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Initializing);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);
    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);

    app->setShouldFail(true, true);
    QMetaObject::invokeMethod(fakeOrbitManager(), "startApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Starting);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Failed);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.count(), 2);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ShuttingDown);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QSignalSpy destroyedSignal(app, SIGNAL(destroyed()));
    app->deleteLater();
    QVERIFY(destroyedSignal.wait(1000));
    // HACK: Why does this even happen??? Likely a Qt(DBus) bug...
    QDBusConnection::sessionBus().registerObject(Hemera::Literals::literal(Hemera::Literals::DBus::orbitManagerPath()), fakeOrbitManager());
}

void HeadlessApplicationBasics::testStopFatals()
{
    Hemera::Test::FakeHeadlessApplication *app = new Hemera::Test::FakeHeadlessApplication;
    QSignalSpy applicationStatus(app, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));
    app->init();
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Initializing);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);
    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);

    app->start();
    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::Stopped);
    QMetaObject::invokeMethod(fakeOrbitManager(), "startApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Starting);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Running);

    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::Running);

    app->setShouldFail(true, true);
    QMetaObject::invokeMethod(fakeOrbitManager(), "stopApplication", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopping);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Failed);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.count(), 2);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ShuttingDown);
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QCOMPARE(app->applicationStatus(), Hemera::Application::ApplicationStatus::ReadyForShutdown);

    QSignalSpy destroyedSignal(app, SIGNAL(destroyed()));
    app->deleteLater();
    QVERIFY(destroyedSignal.wait(1000));
}

void HeadlessApplicationBasics::cleanupTestCase()
{
    TestWithApplicationEnvironment::cleanupTestCaseImpl();
}

QTEST_MAIN(HeadlessApplicationBasics)
#include "headless-application-basics.cpp.moc.hpp"
