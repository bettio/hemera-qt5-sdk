#ifndef HEMERACXX_PROXYQT5APPLICATION_H
#define HEMERACXX_PROXYQT5APPLICATION_H

#include <HemeraCore/Application>

namespace HemeraCxx {

class Application;

class ProxyQt5ApplicationPrivate;
class ProxyQt5Application : public Hemera::Application
{
    Q_OBJECT
    Q_DISABLE_COPY(ProxyQt5Application)
    Q_DECLARE_PRIVATE_D(d_h_ptr, ProxyQt5Application)

public:
    explicit ProxyQt5Application(HemeraCxx::Application *cxxObject, Hemera::ApplicationProperties *properties);
    virtual ~ProxyQt5Application();

protected Q_SLOTS:
    virtual void initImpl() override final;
    virtual void startImpl() override final;
    virtual void stopImpl() override final;
    virtual void prepareForShutdown() override final;

private Q_SLOTS:
    void onApplicationStatusChanged(Hemera::Application::ApplicationStatus status);

private:
    friend class HemeraCxx::Application;
};
}

#endif // HEMERACXX_PROXYQT5APPLICATION_H
