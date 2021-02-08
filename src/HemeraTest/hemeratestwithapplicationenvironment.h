/*
 *
 */

#ifndef HEMERA_TEST_TESTWITHAPPLICATIONENVIRONMENT_H
#define HEMERA_TEST_TESTWITHAPPLICATIONENVIRONMENT_H

#include <HemeraTest/Test>

namespace Hemera {

namespace Test {

class FakeGravityCenter;
class FakeParsec;

class TestWithApplicationEnvironment : public Hemera::Test::Test
{
    Q_OBJECT

public:
    TestWithApplicationEnvironment(QObject* parent = 0);
    virtual ~TestWithApplicationEnvironment();

    Hemera::Test::FakeGravityCenter *fakeGravityCenter();
    Hemera::Test::FakeParsec *fakeParsec();

protected Q_SLOTS:
    virtual void initTestCaseImpl() Q_DECL_OVERRIDE;
    virtual void cleanupTestCaseImpl() Q_DECL_OVERRIDE;

private:
    class Private;
    Private * const d;
};

}

}

#endif // HEMERA_TEST_TESTWITHAPPLICATION_H
