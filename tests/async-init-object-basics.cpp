#include <HemeraTest/Test>

#include <QtCore/QObject>
#include <QTimer>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/Literals>

using namespace Hemera;

class MyAsyncObject : public Hemera::AsyncInitObject
{
    Q_OBJECT

public:
    explicit MyAsyncObject(QObject* parent) : AsyncInitObject(parent) {}
    virtual ~MyAsyncObject() {}

protected:
    void initImpl() Q_DECL_OVERRIDE;
};

class MyAsyncPartObject : public Hemera::AsyncInitObject
{
    Q_OBJECT

public:
    explicit MyAsyncPartObject(bool shouldFail, bool setPart, QObject* parent) 
        : AsyncInitObject(parent), m_shouldFail(shouldFail) { if (setPart) setParts(3); }
    virtual ~MyAsyncPartObject() {}

protected:
    void initImpl() Q_DECL_OVERRIDE;

private:
    bool m_shouldFail;
};

class MyAsyncFailObject : public Hemera::AsyncInitObject
{
    Q_OBJECT

public:
    explicit MyAsyncFailObject(QObject* parent) : AsyncInitObject(parent) {}
    virtual ~MyAsyncFailObject() {}

protected:
    void initImpl() Q_DECL_OVERRIDE;
};

class TestAsyncInitObjectBasics : public Hemera::Test::Test
{
    Q_OBJECT

public:
    TestAsyncInitObjectBasics(QObject *parent = 0)
        : Test(parent)
    { }

private Q_SLOTS:
    void initTestCase();
    void init();

    void testInitSuccess();
    void testInitFailure();
    void testReinit();
    void testSameOperation();
    void testPart();
    void testPartFail();
    void testPartOnNonPart();

    void cleanup();
    void cleanupTestCase();
};

void MyAsyncObject::initImpl()
{
    setReady();
}

void MyAsyncFailObject::initImpl()
{
    setInitError(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()), QStringLiteral("This will always fail!"));
}

void MyAsyncPartObject::initImpl()
{
    QTimer::singleShot(0, this, SLOT(setOnePartIsReady()));
    QTimer::singleShot(0, this, SLOT(setOnePartIsReady()));
    QTimer::singleShot(0, this, SLOT(setOnePartIsReady()));
    if (m_shouldFail) {
        setInitError(Literals::literal(Literals::Errors::badRequest()), QStringLiteral("This will always fail!"));
    }
}

void TestAsyncInitObjectBasics::initTestCase()
{
    initTestCaseImpl();
}

void TestAsyncInitObjectBasics::init()
{
    initImpl();
}

void TestAsyncInitObjectBasics::testInitSuccess()
{
    Hemera::AsyncInitObject *object = new MyAsyncObject(this);

    Hemera::Operation *op = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isValid());
    QVERIFY(!op->isError());
    QVERIFY(op->errorMessage().isEmpty());
    QVERIFY(op->errorName().isEmpty());

    QVERIFY(object->isReady());
    QVERIFY(!object->hasInitError());
    QVERIFY(object->initError().isEmpty());
    QVERIFY(object->initErrorMessage().isEmpty());

    object->deleteLater();
}

void TestAsyncInitObjectBasics::testInitFailure()
{
    Hemera::AsyncInitObject *object = new MyAsyncFailObject(this);

    Hemera::Operation *op = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(!op->isValid());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()));
    QCOMPARE(op->errorMessage(), QStringLiteral("This will always fail!"));

    QVERIFY(!object->isReady());
    QVERIFY(object->hasInitError());
    QCOMPARE(object->initError(), Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()));
    QCOMPARE(object->initErrorMessage(), QStringLiteral("This will always fail!"));

    object->deleteLater();
}

void TestAsyncInitObjectBasics::testReinit()
{
    Hemera::AsyncInitObject *object = new MyAsyncObject(this);

    Hemera::Operation *op = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isValid());

    QVERIFY(object->isReady());

    // Keep it 0. QTest doesn't like nullptr.
    QVERIFY(!object->init());

    object->deleteLater();
}

void TestAsyncInitObjectBasics::testSameOperation()
{
    Hemera::AsyncInitObject *object = new MyAsyncObject(this);

    Hemera::Operation *op = object->init();
    Hemera::Operation *op2 = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QCOMPARE(op, op2);

    QVERIFY(op->isFinished());
    QVERIFY(op->isValid());
    QVERIFY(op2->isFinished());
    QVERIFY(op2->isValid());

    QVERIFY(object->isReady());
    QVERIFY(!object->hasInitError());

    object->deleteLater();
}

void TestAsyncInitObjectBasics::testPart()
{
    Hemera::AsyncInitObject *object = new MyAsyncPartObject(false, true, this);

    Hemera::Operation *op = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isValid());
    QVERIFY(!op->isError());
    QVERIFY(op->errorMessage().isEmpty());
    QVERIFY(op->errorName().isEmpty());

    QVERIFY(object->isReady());
    QVERIFY(!object->hasInitError());
    QVERIFY(object->initError().isEmpty());
    QVERIFY(object->initErrorMessage().isEmpty());

    object->deleteLater();
}

void TestAsyncInitObjectBasics::testPartFail()
{
    Hemera::AsyncInitObject *object = new MyAsyncPartObject(true, true, this);

    Hemera::Operation *op = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(!op->isValid());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()));
    QCOMPARE(op->errorMessage(), QStringLiteral("This will always fail!"));

    QVERIFY(!object->isReady());
    QVERIFY(object->hasInitError());
    QCOMPARE(object->initError(), Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()));
    QCOMPARE(object->initErrorMessage(), QStringLiteral("This will always fail!"));

    object->deleteLater();
}

void TestAsyncInitObjectBasics::testPartOnNonPart()
{
    Hemera::AsyncInitObject *object = new MyAsyncPartObject(false, false, this);

    Hemera::Operation *op = object->init();
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(!op->isValid());
    QVERIFY(op->isError());
    QCOMPARE(op->errorName(), Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()));

    QVERIFY(!object->isReady());
    QVERIFY(object->hasInitError());
    QCOMPARE(object->initError(), Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()));

    object->deleteLater();
}

void TestAsyncInitObjectBasics::cleanup()
{
    cleanupImpl();
}

void TestAsyncInitObjectBasics::cleanupTestCase()
{
    cleanupTestCaseImpl();
}

QTEST_MAIN(TestAsyncInitObjectBasics)
#include "async-init-object-basics.cpp.moc.hpp"
