#include "hemeratest.h"

#include <cstdlib>

#include <QtCore/QTimer>

Q_LOGGING_CATEGORY(LOG_HEMERA_TEST, "Hemera::Test")

namespace Hemera {

namespace Test {

Test::Test(QObject *parent)
    : QObject(parent), mLoop(new QEventLoop(this))
{
    QTimer::singleShot(10 * 60 * 1000, this, SLOT(onWatchdog()));
}

Test::~Test()
{
    delete mLoop;
}

void Test::initTestCaseImpl()
{
}

void Test::initImpl()
{
}

void Test::cleanupImpl()
{
}

void Test::cleanupTestCaseImpl()
{
    // To allow for cleanup code to run (e.g. Hemera::Operation cleanup after they finish)
    mLoop->processEvents();
}

void Test::expectSuccessfulCall(Hemera::Operation *op)
{
    if (op->isError()) {
        qCWarning(LOG_HEMERA_TEST).nospace() << op->errorName()
            << ": " << op->errorMessage();
        mLoop->exit(1);
        return;
    }

    mLoop->exit(0);
}

void Test::expectFailure(Hemera::Operation *op)
{
    if (!op->isError()) {
        qCWarning(LOG_HEMERA_TEST) << "expectFailure(): should have been an error, but wasn't";
        mLastError = QString();
        mLastErrorMessage = QString();
        mLoop->exit(1);
        return;
    }

    mLastError = op->errorName();
    mLastErrorMessage = op->errorMessage();

    mLoop->exit(0);
}

void Test::expectSuccessfulProperty(Hemera::Operation *op)
{
    if (op->isError()) {
        qCWarning(LOG_HEMERA_TEST).nospace() << op->errorName()
            << ": " << op->errorMessage();
        mPropertyValue = QVariant();
        mLoop->exit(1001);
    } else {
//         Tp::PendingVariant *pv = qobject_cast<Tp::PendingVariant*>(op);
//         mPropertyValue = pv->result();
        mLoop->exit(1000);
    }
}

void Test::onWatchdog()
{
    // We can't use QFAIL because the test would then go to cleanup() and/or cleanupTestCase(),
    // which would often hang too - so let's just use abort
    qCWarning(LOG_HEMERA_TEST) << "Test took over 10 minutes to finish, it's probably hung up - aborting";
    std::abort();
}

}

}
