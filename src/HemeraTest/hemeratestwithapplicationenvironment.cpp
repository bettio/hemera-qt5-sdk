#include "hemeratestwithapplicationenvironment.h"

#include <HemeraTest/FakeGravityCenter>
#include <HemeraTest/FakeParsec>

#include <HemeraCore/Application>
#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/Gravity>
#include <HemeraCore/Operation>

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>

#include <QtTest/QSignalSpy>

namespace Hemera {

namespace Test {


class TestWithApplicationEnvironment::Private
{
public:
    Hemera::Test::FakeGravityCenter *fakeGravityCenter;
    Hemera::Test::FakeParsec *fakeParsec;
    QThread *thread;
};

TestWithApplicationEnvironment::TestWithApplicationEnvironment(QObject* parent)
    : Test(parent)
    , d(new Private)
{

}

TestWithApplicationEnvironment::~TestWithApplicationEnvironment()
{
    delete d;
}

void TestWithApplicationEnvironment::initTestCaseImpl()
{
    QVERIFY(!Application::instance());

    d->thread = new QThread(this);
    d->fakeGravityCenter = new Hemera::Test::FakeGravityCenter;
    d->fakeGravityCenter->moveToThread(d->thread);
    d->fakeParsec = new Hemera::Test::FakeParsec;
    d->fakeParsec->moveToThread(d->thread);

    connect(d->fakeParsec, &Hemera::Test::FakeParsec::ready, mLoop, &QEventLoop::quit);
    connect(d->fakeGravityCenter, &Hemera::Test::FakeGravityCenter::ready, d->fakeParsec, &Hemera::Test::FakeParsec::init);
    connect(d->thread, &QThread::started, d->fakeGravityCenter, &Hemera::Test::FakeGravityCenter::init);

    d->thread->start();
    d->thread->setPriority(QThread::LowestPriority);

    QCOMPARE(mLoop->exec(), 0);

    Hemera::Test::Test::initTestCaseImpl();
}

void TestWithApplicationEnvironment::cleanupTestCaseImpl()
{
    connect(d->fakeGravityCenter, &QObject::destroyed, d->thread, &QThread::quit);
    connect(d->fakeParsec, &QObject::destroyed, d->fakeGravityCenter, &QObject::deleteLater, Qt::QueuedConnection);

    connect(d->thread, &QThread::finished, [this] () {
        // Slow executions (namely, Valgrind) might suffer from the concurrency of loops. This way,
        // we're sure the main loop is synced.
        QMetaObject::invokeMethod(mLoop, "quit", Qt::QueuedConnection);
    });

    QMetaObject::invokeMethod(d->fakeParsec, "deleteLater", Qt::QueuedConnection);
    QCOMPARE(mLoop->exec(), 0);

    d->thread->deleteLater();

    Hemera::Test::Test::cleanupTestCaseImpl();
}

FakeGravityCenter* TestWithApplicationEnvironment::fakeGravityCenter()
{
    return d->fakeGravityCenter;
}

FakeParsec* TestWithApplicationEnvironment::fakeParsec()
{
    return d->fakeParsec;
}

}

}
