#ifndef HEMERA_TESTS_TEST_H
#define HEMERA_TESTS_TEST_H

#include <QtCore/QEventLoop>

#include <QtCore/QLoggingCategory>
#include <QtTest/QTest>

#include <HemeraCore/Operation>

namespace Hemera {

namespace Test {

class Test : public QObject
{
    Q_OBJECT

public:

    Test(QObject *parent = 0);

    virtual ~Test();

    QEventLoop *mLoop;

    // The last error received in expectFailure()
    QString mLastError;
    QString mLastErrorMessage;

protected:
//     template<typename T> bool waitForProperty(Tp::PendingVariant *pv, T *value);

protected Q_SLOTS:
    void expectSuccessfulCall(Hemera::Operation*);
    void expectFailure(Hemera::Operation*);
    void expectSuccessfulProperty(Hemera::Operation *op);
    void onWatchdog();

    virtual void initTestCaseImpl();
    virtual void initImpl();

    virtual void cleanupImpl();
    virtual void cleanupTestCaseImpl();

private:
    // The property retrieved by expectSuccessfulProperty()
    QVariant mPropertyValue;
};

// template<typename T>
// bool Test::waitForProperty(Tp::PendingVariant *pv, T *value)
// {
//     connect(pv,
//             SIGNAL(finished(Hemera::Operation*)),
//             SLOT(expectSuccessfulProperty(Hemera::Operation*)));
//     if (mLoop->exec() == 1000) {
//         *value = qdbus_cast<T>(mPropertyValue);
//         return true;
//     }
//     else {
//         *value = T();
//         return false;
//     }
// }

}

}

Q_DECLARE_LOGGING_CATEGORY(LOG_HEMERA_TEST)

#define TEST_VERIFY_OP(op) \
    if (!op->isFinished()) { \
        qCWarning(LOG_HEMERA_TEST) << "unfinished"; \
        mLoop->exit(1); \
        return; \
    } \
    if (op->isError()) { \
        qCWarning(LOG_HEMERA_TEST).nospace() << op->errorName() << ": " << op->errorMessage(); \
        mLoop->exit(2); \
        return; \
    } \
    if (!op->isValid()) { \
        qCWarning(LOG_HEMERA_TEST) << "inconsistent results"; \
        mLoop->exit(3); \
        return; \
    } \
    qCDebug(LOG_HEMERA_TEST) << "finished";

#endif // _TelepathyQt_tests_lib_test_h_HEADER_GUARD_
