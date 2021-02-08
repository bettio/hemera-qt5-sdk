#include "hemerafakeheadlessapplication.h"

#include <HemeraCore/ApplicationProperties>

using namespace Hemera::Test;

class FakeProperties : public Hemera::ApplicationProperties
{
public:
    explicit FakeProperties() {
        setId(QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication"));
        setName(QStringLiteral("Fake Headless Application"));
        setOrganization(QStringLiteral("Ispirata"));
        setVersion(QStringLiteral("0.7.3"));
    }
    virtual ~FakeProperties() {}
};

FakeHeadlessApplication::FakeHeadlessApplication()
    : HeadlessApplication(new FakeProperties)
    , m_shouldFail(false)
    , m_shouldFatal(false)
{
    setenv("WATCHDOG_USEC", "1000", 0);
}

FakeHeadlessApplication::~FakeHeadlessApplication()
{

}

void FakeHeadlessApplication::setShouldFail(bool shouldFail, bool fatal)
{
    m_shouldFail = shouldFail;
    m_shouldFatal = fatal;
}

Hemera::Gravity *FakeHeadlessApplication::publicGravity() const
{
    return gravity();
}

void FakeHeadlessApplication::stopImpl()
{
    // This should fail
    setStarted();
    setReadyForShutdown();

    if (m_shouldFail) {
        if (m_shouldFatal) {
            setStopped(Application::OperationResult::Fatal);
        } else {
            setStopped(Application::OperationResult::Failed);
        }
    } else {
        setStopped();
    }
}

void FakeHeadlessApplication::startImpl()
{
    // This should fail
    setStopped();

    if (m_shouldFail) {
        if (m_shouldFatal) {
            setStarted(Application::OperationResult::Fatal);
        } else {
            setStarted(Application::OperationResult::Failed);
        }
    } else {
        setStarted();
    }
}

void FakeHeadlessApplication::prepareForShutdown()
{
    if (m_shouldFail) {
        if (m_shouldFatal) {
            setReadyForShutdown(Application::OperationResult::Fatal);
        } else {
            setReadyForShutdown(Application::OperationResult::Failed);
        }
    } else {
        Hemera::Application::prepareForShutdown();
    }
}

void FakeHeadlessApplication::initImpl()
{
    setReady();
}

#ifdef BUILD_AS_STANDALONE_APPS
HEMERA_HEADLESS_APPLICATION_MAIN(FakeHeadlessApplication)
#endif
