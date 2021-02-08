#include "hemeracxxproxyqt5application.h"

#include "hemeracxxapplication_p.h"

#include "hemeraapplication_p.h"

namespace HemeraCxx {

class ProxyQt5ApplicationPrivate : public Hemera::ApplicationPrivate
{
public:
    ProxyQt5ApplicationPrivate(ProxyQt5Application *q) : Hemera::ApplicationPrivate(q) {}
    virtual ~ProxyQt5ApplicationPrivate() {}

    Q_DECLARE_PUBLIC(ProxyQt5Application)

    virtual void initHook() Q_DECL_OVERRIDE Q_DECL_FINAL;

    HemeraCxx::Application *cxxObject;
};

void ProxyQt5ApplicationPrivate::initHook()
{
    if (cxxObject->d->pre_init != nullptr) {
        cxxObject->d->pre_init(cxxObject->d->pre_init_data);
    }

    Hemera::ApplicationPrivate::initHook();
}

ProxyQt5Application::ProxyQt5Application(HemeraCxx::Application* cxxObject, Hemera::ApplicationProperties* properties)
    : Application(*new ProxyQt5ApplicationPrivate(this), properties)
{
    Q_D(ProxyQt5Application);
    d->cxxObject = cxxObject;

    // Connect signal->callback handlers
    connect(this, &Hemera::Application::applicationStatusChanged, this, &ProxyQt5Application::onApplicationStatusChanged);
}

ProxyQt5Application::~ProxyQt5Application()
{
}

void ProxyQt5Application::onApplicationStatusChanged(Hemera::Application::ApplicationStatus status)
{
    Q_D(ProxyQt5Application);
    d->cxxObject->d->onApplicationStatusChanged(static_cast<HemeraCxx::Application::ApplicationStatus>(status));
}

void ProxyQt5Application::initImpl()
{
    Q_D(ProxyQt5Application);
    d->cxxObject->initImpl();
}

void ProxyQt5Application::startImpl()
{
    Q_D(ProxyQt5Application);
    d->cxxObject->startImpl();
}

void ProxyQt5Application::stopImpl()
{
    Q_D(ProxyQt5Application);
    d->cxxObject->stopImpl();
}

void ProxyQt5Application::prepareForShutdown()
{
    Q_D(ProxyQt5Application);
    d->cxxObject->prepareForShutdown();
}


}
