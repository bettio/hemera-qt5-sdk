#ifndef HEMERA_TESTS_HEMERAFAKEHEADLESSAPPLICATION_H
#define HEMERA_TESTS_HEMERAFAKEHEADLESSAPPLICATION_H

#include <HemeraCore/HeadlessApplication>

namespace Hemera {

namespace Test {

class FakeHeadlessApplication : public Hemera::HeadlessApplication
{

public:
    explicit FakeHeadlessApplication();
    virtual ~FakeHeadlessApplication();

    Hemera::Gravity *publicGravity() const;

    // Backdoors
public Q_SLOTS:
    void setShouldFail(bool shouldFail, bool fatal = false);

protected:
    virtual void stopImpl();
    virtual void startImpl();
    virtual void prepareForShutdown();
    virtual void initImpl();

private:
    bool m_shouldFail;
    bool m_shouldFatal;

    friend class HeadlessApplicationBasics;
};

}

}

#endif // HEMERA_TESTS_HEMERAFAKEHEADLESSAPPLICATION_H
