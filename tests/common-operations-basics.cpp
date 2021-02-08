#include <HemeraTest/DBusTest>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QJsonDocument>
#include <QJsonObject>

#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusInterface>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

using namespace Hemera;

class AnObjectOnDBus : public AsyncInitDBusObject
{
    Q_OBJECT

public:
    explicit AnObjectOnDBus(QObject* parent) : AsyncInitDBusObject(parent) {}
    virtual ~AnObjectOnDBus() {}

    void setShouldFail(bool shouldFail) { m_shouldFail = shouldFail; }

    // DBus Methods
public Q_SLOTS:
    void callVoid() { if (m_shouldFail) sendErrorReply(Literals::literal(Literals::Errors::unhandledRequest())); }
    uint callUInt() { if (m_shouldFail) sendErrorReply(Literals::literal(Literals::Errors::unhandledRequest())); else return 42; return 0; }
    QStringList callStringList() {
        if (m_shouldFail) {
            sendErrorReply(Literals::literal(Literals::Errors::unhandledRequest()));
        } else {
            QStringList ret;
            ret << QStringLiteral("a") << QStringLiteral("test") << QStringLiteral("list");
            return ret;
        }
        return QStringList();
    }
    QVariantMap callVariantMap() {
        if (m_shouldFail) {
            sendErrorReply(Literals::literal(Literals::Errors::unhandledRequest()));
        } else {
            QVariantMap ret;
            ret.insert(QStringLiteral("number"), 42);
            ret.insert(QStringLiteral("boolean"), true);
            return ret;
        }
        return QVariantMap();
    }
    QByteArray callJson() {
        if (m_shouldFail) {
            sendErrorReply(Literals::literal(Literals::Errors::unhandledRequest()));
        } else {
            QVariantMap map;
            map.insert(QStringLiteral("number"), 42);
            map.insert(QStringLiteral("boolean"), true);
            return QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
        }
        return QByteArray();
    }

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    bool m_shouldFail;
};

class AnObjectOnDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.ispirata.Hemera.Test.AnObjectOnDBus")
public:
    AnObjectOnDBusAdaptor(AnObjectOnDBus *parent) : QDBusAbstractAdaptor(parent) { setAutoRelaySignals(true); }
    virtual ~AnObjectOnDBusAdaptor() {}

    inline AnObjectOnDBus *parent() const
    { return static_cast<AnObjectOnDBus *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void callVoid() { parent()->callVoid(); }
    uint callUInt() { return parent()->callUInt(); }
    QStringList callStringList() { return parent()->callStringList(); }
    QVariantMap callVariantMap() { return parent()->callVariantMap(); }
    QByteArray callJson() { return parent()->callJson(); }
Q_SIGNALS: // SIGNALS
};

class CommonOperationsBasics : public Hemera::Test::DBusTest
{
    Q_OBJECT

public:
    CommonOperationsBasics(QObject *parent = 0)
        : DBusTest(parent)
    { }

private Q_SLOTS:
    void initTestCase();
    void init();

    void testDBusVoid();
    void testDBusUInt();
    void testDBusStringList();
    void testDBusVariantMap();
    void testDBusJson();
    void testComposite();
    void testCompositeEmpty();
    void testMismatch();
    void testFullDeletion();

    void cleanup();
    void cleanupTestCase();

private:
    QList< QPointer<Hemera::Operation> > m_allOperations;
    AnObjectOnDBus *m_object;
    QThread *m_thread;
    QDBusInterface *m_iface;
};

void AnObjectOnDBus::initImpl()
{
    QVERIFY(QDBusConnection::systemBus().registerService(QStringLiteral("com.ispirata.Hemera.Test.AnObjectOnDBus")));
    QVERIFY(QDBusConnection::systemBus().registerObject(QStringLiteral("/AnObjectOnDBus"), this));

    new AnObjectOnDBusAdaptor(this);

    setReady();
}

void CommonOperationsBasics::initTestCase()
{
    initTestCaseImpl();

    m_object = new AnObjectOnDBus(nullptr);
    m_thread = new QThread(this);
    m_object->moveToThread(m_thread);
    connect(m_thread, &QThread::started, m_object, &AnObjectOnDBus::init);

    QEventLoop e;
    connect(m_object, &AnObjectOnDBus::ready, &e, &QEventLoop::quit);
    m_thread->start();

    m_thread->setPriority(QThread::LowestPriority);

    QCOMPARE(e.exec(), 0);
    QVERIFY(m_object->isReady());

    m_iface = new QDBusInterface(QStringLiteral("com.ispirata.Hemera.Test.AnObjectOnDBus"),
                                 QStringLiteral("/AnObjectOnDBus"),
                                 QStringLiteral("com.ispirata.Hemera.Test.AnObjectOnDBus"),
                                 QDBusConnection::systemBus(), this);

    QVERIFY(m_iface->isValid());
}

void CommonOperationsBasics::init()
{
    initImpl();

    m_object->setShouldFail(false);
}

void CommonOperationsBasics::testDBusVoid()
{
    Hemera::DBusVoidOperation *op = new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());

    m_object->setShouldFail(true);

    op = new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Literals::literal(Literals::Errors::unhandledRequest()));
}

void CommonOperationsBasics::testDBusUInt()
{
    Hemera::DBusUIntOperation *op = new DBusUIntOperation(m_iface->asyncCall(QStringLiteral("callUInt")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QCOMPARE(op->result(), (uint)42);

    m_object->setShouldFail(true);

    op = new DBusUIntOperation(m_iface->asyncCall(QStringLiteral("callUInt")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Literals::literal(Literals::Errors::unhandledRequest()));
}

void CommonOperationsBasics::testDBusStringList()
{
    Hemera::DBusStringListOperation *op = new DBusStringListOperation(m_iface->asyncCall(QStringLiteral("callStringList")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QStringList res = op->result();
    QCOMPARE(res.count(), 3);
    QCOMPARE(res.at(0), QStringLiteral("a"));
    QCOMPARE(res.at(1), QStringLiteral("test"));
    QCOMPARE(res.at(2), QStringLiteral("list"));

    m_object->setShouldFail(true);

    op = new DBusStringListOperation(m_iface->asyncCall(QStringLiteral("callStringList")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Literals::literal(Literals::Errors::unhandledRequest()));
}

void CommonOperationsBasics::testDBusVariantMap()
{
    Hemera::DBusVariantMapOperation *op = new DBusVariantMapOperation(m_iface->asyncCall(QStringLiteral("callVariantMap")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVariantMap res = op->result();
    QCOMPARE(res.count(), 2);
    QCOMPARE(res.value(QStringLiteral("number")).toUInt(), (uint)42);
    QCOMPARE(res.value(QStringLiteral("boolean")).toBool(), true);

    m_object->setShouldFail(true);

    op = new DBusVariantMapOperation(m_iface->asyncCall(QStringLiteral("callVariantMap")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Literals::literal(Literals::Errors::unhandledRequest()));
}

void CommonOperationsBasics::testDBusJson()
{
    Hemera::DBusJsonOperation *op = new DBusJsonOperation(m_iface->asyncCall(QStringLiteral("callJson")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QJsonDocument res = op->result();
    QJsonObject object = res.object();
    QCOMPARE(object.count(), 2);
    QCOMPARE(object.value(QStringLiteral("number")).toDouble(), (double)42);
    QCOMPARE(object.value(QStringLiteral("boolean")).toBool(), true);

    m_object->setShouldFail(true);

    op = new DBusJsonOperation(m_iface->asyncCall(QStringLiteral("callJson")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Literals::literal(Literals::Errors::unhandledRequest()));
}

void CommonOperationsBasics::testComposite()
{
    QList<Hemera::Operation*> opList;
    opList.append(new DBusVariantMapOperation(m_iface->asyncCall(QStringLiteral("callVariantMap"))));
    opList.append(new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid"))));
    opList.append(new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid"))));
    opList.append(new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid"))));

    Hemera::CompositeOperation *op = new CompositeOperation(opList);
    m_allOperations.append(op);
    for (Hemera::Operation *o : opList) {
        m_allOperations.append(o);
    }
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());

    m_object->setShouldFail(true);

    opList.clear();
    opList.append(new DBusVariantMapOperation(m_iface->asyncCall(QStringLiteral("callVariantMap"))));
    opList.append(new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid"))));
    opList.append(new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid"))));
    opList.append(new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callVoid"))));

    op = new CompositeOperation(opList);
    m_allOperations.append(op);
    for (Hemera::Operation *o : opList) {
        m_allOperations.append(o);
    }
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Literals::literal(Literals::Errors::unhandledRequest()));
}

void CommonOperationsBasics::testCompositeEmpty()
{
    Hemera::CompositeOperation *op = new CompositeOperation(QList<Hemera::Operation*>());
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(!op->isError());
}

void CommonOperationsBasics::testMismatch()
{
    Hemera::Operation *op = new DBusVariantMapOperation(m_iface->asyncCall(QStringLiteral("callUInt")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), QDBusError::errorString(QDBusError::InvalidSignature));

    op = new DBusVoidOperation(m_iface->asyncCall(QStringLiteral("callUInt")));
    m_allOperations.append(op);
    // Void should be always ok
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    op = new DBusStringListOperation(m_iface->asyncCall(QStringLiteral("callUInt")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), QDBusError::errorString(QDBusError::InvalidSignature));

    op = new DBusUIntOperation(m_iface->asyncCall(QStringLiteral("callVariantMap")));
    m_allOperations.append(op);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), QDBusError::errorString(QDBusError::InvalidSignature));
}

void CommonOperationsBasics::testFullDeletion()
{
    QTimer::singleShot(3000, mLoop, SLOT(quit()));
    mLoop->exec();

    for (QPointer<Hemera::Operation> op : m_allOperations) {
        QVERIFY(op.isNull());
    }
}

void CommonOperationsBasics::cleanup()
{
    cleanupImpl();
}

void CommonOperationsBasics::cleanupTestCase()
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

QTEST_MAIN(CommonOperationsBasics)
#include "common-operations-basics.cpp.moc.hpp"
