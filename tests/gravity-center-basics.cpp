#include <HemeraTest/FakeGravityCenter>
#include <HemeraTest/FakeOrbitManager>
#include <HemeraTest/Test>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/Gravity>
#include <HemeraCore/Operation>

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QtTest/QSignalSpy>
#include <QtTest/QTest>


using namespace Hemera;


class TestFakeGravity : public Hemera::Test::Test
{
    Q_OBJECT

public:
    TestFakeGravity(QObject *parent = nullptr)
        : Test(parent)
        , m_fakeGravityCenter(new Hemera::Test::FakeGravityCenter(nullptr))
        , m_fakeOrbitManager(new Hemera::Test::FakeOrbitManager(nullptr))
        , m_gravityCenter(new Gravity(nullptr))
        , m_thread(nullptr)
    {
    }

private Q_SLOTS:
    void initTestCase();        // inits the necessaary for test to execute

    void testActiveOrbit();
    void testSwitchActiveOrbit();

    void cleanupTestCase();

private:
    Hemera::Test::FakeGravityCenter *m_fakeGravityCenter;
    Hemera::Test::FakeOrbitManager *m_fakeOrbitManager;
    Gravity *m_gravityCenter;
    QThread *m_thread;
};


void TestFakeGravity::cleanupTestCase()
{
    // remove fake orbit control
    m_gravityCenter->deleteLater();

    connect(m_fakeGravityCenter, &QObject::destroyed, m_fakeOrbitManager, &QObject::deleteLater);
    connect(m_fakeOrbitManager, &QObject::destroyed, m_thread, &QThread::quit);

    connect(m_thread, &QThread::finished, [this] () {
        // Slow executions (namely, Valgrind) might suffer from the concurrency of loops. This way,
        // we're sure the main loop is synced.
        QMetaObject::invokeMethod(mLoop, "quit", Qt::QueuedConnection);
    });

    QMetaObject::invokeMethod(m_fakeGravityCenter, "deleteLater", Qt::QueuedConnection);
    QCOMPARE(mLoop->exec(), 0);

    m_thread->deleteLater();
}


void TestFakeGravity::initTestCase()
{
    m_thread = new QThread(this);
    m_fakeGravityCenter->moveToThread(m_thread);
    m_fakeOrbitManager->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_fakeGravityCenter, &Hemera::Test::FakeGravityCenter::init);
    connect(m_fakeGravityCenter, &Hemera::Test::FakeGravityCenter::ready, m_fakeOrbitManager, &Hemera::Test::FakeOrbitManager::init);
    connect(m_fakeOrbitManager, &Hemera::Test::FakeOrbitManager::ready, m_gravityCenter, &Hemera::Gravity::init);
    connect(m_gravityCenter, &Hemera::Gravity::ready, mLoop, &QEventLoop::quit);

    m_thread->start();
    m_thread->setPriority(QThread::LowestPriority);

    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(m_fakeGravityCenter->isReady());
    QVERIFY(m_fakeOrbitManager->isReady());
    QVERIFY(m_gravityCenter->isReady());
}


void TestFakeGravity::testActiveOrbit()
{
    QCOMPARE(m_gravityCenter->activeOrbit(), QStringLiteral("none"));
}


void TestFakeGravity::testSwitchActiveOrbit()
{
    QSignalSpy activeChanged(m_gravityCenter, SIGNAL(activeOrbitChanged(QString)));

    Hemera::Operation *op = m_gravityCenter->requestOrbitSwitch(QStringLiteral("thisIsANewOrbit"));
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isValid());

    QCOMPARE(activeChanged.count(), 1);
    QCOMPARE(m_gravityCenter->activeOrbit(), QStringLiteral("thisIsANewOrbit"));
}



QTEST_MAIN(TestFakeGravity)
#include "gravity-center-basics.cpp.moc.hpp"
