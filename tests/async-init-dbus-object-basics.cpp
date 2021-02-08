#include <HemeraTest/DBusTest>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QThread>

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingReply>

#include <QtTest/QSignalSpy>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/Literals>

using namespace Hemera;

class MyAsyncObjectOnDBus : public Hemera::AsyncInitDBusObject
{
    Q_OBJECT
    Q_PROPERTY(uint meaningOfLife READ meaningOfLife)
    Q_PROPERTY(QString volatileProp READ volatileProp WRITE setVolatileProp NOTIFY volatilePropChanged)
    Q_PROPERTY(uint prop1 READ prop1 NOTIFY twoPropsChanged)
    Q_PROPERTY(uint prop2 READ prop2 NOTIFY twoPropsChanged)
public:
    explicit MyAsyncObjectOnDBus(QObject* parent) : AsyncInitDBusObject(parent), m_vProp(QStringLiteral("startValue")) {}
    virtual ~MyAsyncObjectOnDBus() {}

    uint meaningOfLife() const { return 42; }
    QString volatileProp() const { return m_vProp; }
    uint prop1() const { return m_prop1; }
    uint prop2() const { return m_prop2; }
public Q_SLOTS:
    void setVolatileProp(const QString &vProp) { m_vProp = vProp; Q_EMIT volatilePropChanged(vProp); }
    void setBothProps(uint a, uint b) { m_prop1 = a; m_prop2 = b; Q_EMIT twoPropsChanged(); }
protected:
    void initImpl() Q_DECL_OVERRIDE;
Q_SIGNALS:
    void volatilePropChanged(const QString &vProp);
    void twoPropsChanged();
private:
    QString m_vProp;
    uint m_prop1;
    uint m_prop2;
};

class TestAsyncInitDBusObjectBasics : public Hemera::Test::DBusTest
{
    Q_OBJECT

public:
    TestAsyncInitDBusObjectBasics(QObject *parent = 0)
        : DBusTest(parent)
    { }

private Q_SLOTS:
    void initTestCase();
    void init();

    void testReadProperties();
    void testPropertyChangedSignal();
    void testChangeTwoPropertiesAtOnce();

    void cleanup();
    void cleanupTestCase();

private:
    Hemera::AsyncInitDBusObject *m_object;
    QThread *m_thread;
    QDBusInterface *m_iface;
};

void MyAsyncObjectOnDBus::initImpl()
{
    QVERIFY(QDBusConnection::systemBus().registerService(QStringLiteral("com.ispirata.Hemera.Test.AsyncInitDBusObjectBasics")));
    QTimer::singleShot(0, this, SLOT(setReady()));
}

void TestAsyncInitDBusObjectBasics::initTestCase()
{
    initTestCaseImpl();

    m_object = new MyAsyncObjectOnDBus(nullptr);
    m_thread = new QThread(this);
    m_object->moveToThread(m_thread);
    connect(m_thread, &QThread::started, m_object, &MyAsyncObjectOnDBus::init);

    QEventLoop e;
    connect(m_object, &MyAsyncObjectOnDBus::ready, &e, &QEventLoop::quit);
    m_thread->start();

    m_thread->setPriority(QThread::LowestPriority);

    QCOMPARE(e.exec(), 0);
    QVERIFY(m_object->isReady());

    m_iface = new QDBusInterface(QStringLiteral("com.ispirata.Hemera.Test.AsyncInitDBusObjectBasics"),
                                 Hemera::Literals::literal(Hemera::Literals::DBus::dbusObjectPath()),
                                 Hemera::Literals::literal(Hemera::Literals::DBus::dbusObjectInterface()),
                                 QDBusConnection::systemBus(), this);
    QVERIFY(m_iface->isValid());
}

void TestAsyncInitDBusObjectBasics::init()
{
    initImpl();
}

void TestAsyncInitDBusObjectBasics::testReadProperties()
{
    QDBusPendingCallWatcher wtc(m_iface->asyncCall(QStringLiteral("allProperties")));
    connect(&wtc, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(expectSuccessfulCall(QDBusPendingCallWatcher*)));
    QCOMPARE(mLoop->exec(), 0);

    QDBusPendingReply<QVariantMap> rpl = wtc;
    QVERIFY(rpl.isValid());

    QVERIFY(rpl.value().contains(QStringLiteral("meaningOfLife")));
    QCOMPARE(rpl.value().value(QStringLiteral("meaningOfLife")).toUInt(), (uint)42);

    QVERIFY(rpl.value().contains(QStringLiteral("volatileProp")));
    QCOMPARE(rpl.value().value(QStringLiteral("volatileProp")).toString(), QStringLiteral("startValue"));
}

void TestAsyncInitDBusObjectBasics::testPropertyChangedSignal()
{
    QSignalSpy propertyChangedSignal(m_object, SIGNAL(propertiesChanged(QVariantMap)));
    QEventLoop e;
    QVERIFY(QDBusConnection::systemBus().connect(QStringLiteral("com.ispirata.Hemera.Test.AsyncInitDBusObjectBasics"),
                                 Hemera::Literals::literal(Hemera::Literals::DBus::dbusObjectPath()),
                                 Hemera::Literals::literal(Hemera::Literals::DBus::dbusObjectInterface()),
                                 QLatin1String("propertiesChanged"), &e, SLOT(quit())));
    // Make it delayed. It's quite hard to follow on the event loop otherwise.
    QMetaObject::invokeMethod(m_object, "setVolatileProp", Qt::QueuedConnection, Q_ARG(const QString&, QStringLiteral("newValue")));
    QCOMPARE(e.exec(), 0);

    QDBusPendingCallWatcher wtc(m_iface->asyncCall(QStringLiteral("allProperties")));
    expectSuccessfulCall(&wtc);
    QDBusPendingReply<QVariantMap> rpl = wtc;
    QVERIFY(rpl.isValid());

    QVERIFY(rpl.value().contains(QStringLiteral("volatileProp")));
    QCOMPARE(rpl.value().value(QStringLiteral("volatileProp")).toString(), QStringLiteral("newValue"));
    QCOMPARE(propertyChangedSignal.count(), 1);
    QVariantMap pSignal = propertyChangedSignal.takeFirst().at(0).toMap();
    QCOMPARE(pSignal.value(QStringLiteral("volatileProp")).toString(), QStringLiteral("newValue"));
}

void TestAsyncInitDBusObjectBasics::testChangeTwoPropertiesAtOnce()
{
    QSignalSpy propertyChangedSignal(m_object, SIGNAL(propertiesChanged(QVariantMap)));
    QEventLoop e;
    QVERIFY(QDBusConnection::systemBus().connect(QStringLiteral("com.ispirata.Hemera.Test.AsyncInitDBusObjectBasics"),
                                 Hemera::Literals::literal(Hemera::Literals::DBus::dbusObjectPath()),
                                 Hemera::Literals::literal(Hemera::Literals::DBus::dbusObjectInterface()),
                                 QLatin1String("propertiesChanged"), &e, SLOT(quit())));
    // Make it delayed. It's quite hard to follow on the event loop otherwise.
    QMetaObject::invokeMethod(m_object, "setBothProps", Qt::QueuedConnection, Q_ARG(uint, 1), Q_ARG(uint, 2));
    QCOMPARE(e.exec(), 0);

    QDBusPendingCallWatcher wtc(m_iface->asyncCall(QStringLiteral("allProperties")));
    expectSuccessfulCall(&wtc);
    QDBusPendingReply<QVariantMap> rpl = wtc;
    QVERIFY(rpl.isValid());

    QCOMPARE(rpl.value().value(QStringLiteral("prop1")).toUInt(), (uint)1);
    QCOMPARE(rpl.value().value(QStringLiteral("prop2")).toUInt(), (uint)2);
    QCOMPARE(propertyChangedSignal.count(), 1);
    QVariantMap pSignal = propertyChangedSignal.takeFirst().at(0).toMap();
    QCOMPARE(pSignal.count(), 2);
    QCOMPARE(pSignal.value(QStringLiteral("prop1")).toUInt(), (uint)1);
    QCOMPARE(pSignal.value(QStringLiteral("prop2")).toUInt(), (uint)2);
}

void TestAsyncInitDBusObjectBasics::cleanup()
{
    cleanupImpl();
}

void TestAsyncInitDBusObjectBasics::cleanupTestCase()
{
    connect(m_object, &QObject::destroyed, m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished, [this] () {
        // Slow executions (namely, Valgrind) might suffer from the concurrency of loops. This way,
        // we're sure the main loop is synced.
        QMetaObject::invokeMethod(mLoop, "quit", Qt::QueuedConnection);
    });
    QMetaObject::invokeMethod(m_object, "deleteLater", Qt::QueuedConnection);
    QCOMPARE(mLoop->exec(), 0);

    m_thread->deleteLater();

    cleanupTestCaseImpl();
}

QTEST_MAIN(TestAsyncInitDBusObjectBasics)
#include "async-init-dbus-object-basics.cpp.moc.hpp"
