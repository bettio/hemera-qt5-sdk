#include <HemeraTest/Test>

#include <QtCore/QObject>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/Literals>

using namespace Hemera;

class MyOperation : public Hemera::Operation
{
    Q_OBJECT

public:
    explicit MyOperation(QObject* parent) : Operation(parent) {}
    virtual ~MyOperation() {}

protected:
    void startImpl() Q_DECL_OVERRIDE { setFinished(); }
};

class MyOperationTwice : public Hemera::Operation
{
    Q_OBJECT

public:
    explicit MyOperationTwice(QObject* parent) : Operation(parent) {}
    virtual ~MyOperationTwice() {}

protected:
    void startImpl() Q_DECL_OVERRIDE { setFinished(); setFinished();
                                       setFinishedWithError(QDBusError(QDBusError::Failed, QStringLiteral("nonono"))); };
};

class MyFailOperation : public Hemera::Operation
{
    Q_OBJECT

public:
    explicit MyFailOperation(QObject* parent) : Operation(parent) {}
    virtual ~MyFailOperation() {}

protected:
    void startImpl() Q_DECL_OVERRIDE { setFinishedWithError(QDBusError(QDBusError::Failed, QStringLiteral("nonono"))); }
};

class MyFailWrongOperation : public Hemera::Operation
{
    Q_OBJECT

public:
    explicit MyFailWrongOperation(QObject* parent) : Operation(parent) {}
    virtual ~MyFailWrongOperation() {}

protected:
    void startImpl() Q_DECL_OVERRIDE { setFinishedWithError(QString(), QString()); }
};

class MyFailTwiceOperation : public Hemera::Operation
{
    Q_OBJECT

public:
    explicit MyFailTwiceOperation(QObject* parent) : Operation(parent) {}
    virtual ~MyFailTwiceOperation() {}

protected:
    void startImpl() Q_DECL_OVERRIDE { setFinishedWithError(QDBusError(QDBusError::Failed, QStringLiteral("nonono"))); 
                                       setFinishedWithError(QDBusError(QDBusError::Failed, QStringLiteral("nonono")));
                                       setFinished(); };
};

class OperationBasics : public Hemera::Test::Test
{
    Q_OBJECT

public:
    OperationBasics(QObject *parent = 0)
        : Test(parent)
    { }

private Q_SLOTS:
    void initTestCase();
    void init();

    void testBasics();
    void testFail();
    void testFailAtFailing();
    void testTwice();
    void testFailTwice();
    void testDelete();

    void cleanup();
    void cleanupTestCase();
};

void OperationBasics::initTestCase()
{
    initTestCaseImpl();
}

void OperationBasics::init()
{
    initImpl();
}

void OperationBasics::testBasics()
{
    Hemera::Operation *op = new MyOperation(this);

    QVERIFY(!op->isFinished());
    QVERIFY(!op->isError());
    QVERIFY(!op->isValid());

    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(!op->isError());
    QVERIFY(op->isValid());
    QVERIFY(op->errorMessage().isEmpty());
    QVERIFY(op->errorName().isEmpty());
}

void OperationBasics::testFail()
{
    Hemera::Operation *op = new MyFailOperation(this);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
}

void OperationBasics::testFailAtFailing()
{
    Hemera::Operation *op = new MyFailWrongOperation(this);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)));
    QCOMPARE(mLoop->exec(), 0);

    QCOMPARE(op->errorName(), Hemera::Literals::literal(Hemera::Literals::Errors::errorHandlingError()));
}

void OperationBasics::testTwice()
{
    Hemera::Operation *op = new MyOperationTwice(this);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectSuccessfulCall(Hemera::Operation*)), Qt::QueuedConnection);
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isValid());
}

void OperationBasics::testFailTwice()
{
    Hemera::Operation *op = new MyFailTwiceOperation(this);
    connect(op, SIGNAL(finished(Hemera::Operation*)), this, SLOT(expectFailure(Hemera::Operation*)), Qt::QueuedConnection);
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(op->isFinished());
    QVERIFY(op->isError());
}

void OperationBasics::testDelete()
{
    Hemera::Operation *op = new MyOperation(this);
    delete op;
}

void OperationBasics::cleanup()
{
    cleanupImpl();
}

void OperationBasics::cleanupTestCase()
{
    cleanupTestCaseImpl();
}

QTEST_MAIN(OperationBasics)
#include "operation-basics.cpp.moc.hpp"
