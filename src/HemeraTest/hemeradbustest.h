#ifndef HEMERA_TESTS_DBUSTEST_H
#define HEMERA_TESTS_DBUSTEST_H

#include <HemeraTest/Test>

#include <QtDBus/QDBusPendingCallWatcher>

namespace Hemera {

namespace Test {

class DBusTest : public Test
{
    Q_OBJECT

public:

    DBusTest(QObject *parent = 0);

    virtual ~DBusTest();

protected:
//     template<typename T> bool waitForProperty(Tp::PendingVariant *pv, T *value);

protected Q_SLOTS:
    void expectSuccessfulCall(QDBusPendingCallWatcher*);

    virtual void initTestCaseImpl() override;
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

Q_DECLARE_LOGGING_CATEGORY(LOG_HEMERA_DBUSTEST)

#endif // HEMERA_TESTS_DBUSTEST_H
