#include "hemeracxxapplication_p.h"

#include "hemeracxxglobal.h"

#include "hemeracxxapplicationproperties.h"
#include "hemeracxxproxyqt5application.h"

#include <QtCore/QDebug>

#include <HemeraCore/Literals>
#include <HemeraCore/Operation>

#include <cstring>

namespace HemeraCxx {

static int _injected_argc;
static char** _injected_argv;

char *convert_str(const std::string &s)
{
    return strdup(s.c_str());
}

void __hemera_init(std::vector< std::string > argv)
{
    std::vector<char*> vc;
    std::transform(argv.begin(), argv.end(), std::back_inserter(vc), convert_str);

    __hemera_init(vc.size(), &vc[0]);
}

void __hemera_init(int argc, char **argv)
{
    if (!QCoreApplication::instance()) {
        _injected_argc = argc;

        _injected_argv = new char*[argc];
        for (int i = 0; i < argc; ++i) {
            _injected_argv[i] = argv[i];
        }

        new QCoreApplication(_injected_argc, _injected_argv);
    }
}

void __hemera_destroy()
{
    delete QCoreApplication::instance();

    for (int i = 0; i < _injected_argc; ++i) {
        free(_injected_argv[i]);
    }

    delete _injected_argv;
}

void Application::Private::onApplicationStatusChanged(HemeraCxx::Application::ApplicationStatus status)
{
    // Invoke every registered callback
    for (const QPair< _application_status_changed_cb, void* > &cb : applicationStatusCbs) {
        cb.first(status, cb.second);
    }
}


class ApplicationHelper
{
public:
    ApplicationHelper() : q(nullptr) {}
    ~ApplicationHelper() {
        // Do not delete - the object will be cleaned up already.
    }
    Application *q;
};

Q_GLOBAL_STATIC(ApplicationHelper, s_globalCxxApplication)

Application *Application::applicationInstance()
{
    if (!s_globalCxxApplication()->q) {
        return nullptr;
    }

    return s_globalCxxApplication()->q;
}

Application::Application(HemeraCxx::ApplicationProperties* properties)
    : d(new Private)
{
    Q_ASSERT(!s_globalCxxApplication()->q);
    s_globalCxxApplication()->q = this;

    d->proxy = new ProxyQt5Application(this, properties->qt5Object());
}

Application::~Application()
{
    s_globalCxxApplication()->q = nullptr;
    delete d;
}

std::string Application::id()
{
    return applicationInstance()->d->proxy->id().toStdString();
}

Application::ApplicationStatus Application::applicationStatus() const
{
    return static_cast<HemeraCxx::Application::ApplicationStatus>(d->proxy->applicationStatus());
}

bool Application::releaseOrbitSwitchInhibition()
{
    return d->proxy->releaseOrbitSwitchInhibition()->synchronize();
}

int Application::registerApplicationStatusChangedCallback(Application::_application_status_changed_cb cb, void* data)
{
    ++d->applicationStatusCbId;
    d->applicationStatusCbs.insert(d->applicationStatusCbId, qMakePair(cb, data));
    return d->applicationStatusCbId;
}

void Application::unregisterApplicationStatusChangedCallback(int cbId)
{
    d->applicationStatusCbs.remove(cbId);
}

bool Application::requestOrbitSwitchInhibition(const std::string& reason)
{
    return d->proxy->requestOrbitSwitchInhibition(QString::fromStdString(reason))->synchronize();
}

std::string Application::resourcePath(const std::string& path, Application::ResourceDomain domain, bool checkPathExistence)
{
    return applicationInstance()->d->proxy->resourcePath(QString::fromStdString(path),
                                                         static_cast<Hemera::Application::ResourceDomain>(domain), checkPathExistence).toStdString();
}

long long int Application::writeToResource(const std::string& path, const char* data, Application::ResourceDomain domain)
{
    return applicationInstance()->d->proxy->writeToResource(QString::fromStdString(path), data, static_cast<Hemera::Application::ResourceDomain>(domain));
}

void Application::setReady(Application::OperationResult result, const std::string& errorName, const std::string& errorMessage)
{
    if (result == OperationResult::Success) {
        d->proxy->setReady();
    } else {
        d->proxy->setInitError(QString::fromStdString(errorName), QString::fromStdString(errorMessage));
    }
}

void Application::setStarted(Application::OperationResult result, const std::string& errorName, const std::string& errorMessage)
{
    d->proxy->setStarted(static_cast<Hemera::Application::OperationResult>(result), QString::fromStdString(errorName), QString::fromStdString(errorMessage));
}

void Application::setStopped(Application::OperationResult result, const std::string& errorName, const std::string& errorMessage)
{
    d->proxy->setStopped(static_cast<Hemera::Application::OperationResult>(result), QString::fromStdString(errorName), QString::fromStdString(errorMessage));
}

void Application::setReadyForShutdown(Application::OperationResult result, const std::string& errorName, const std::string& errorMessage)
{
    d->proxy->setReadyForShutdown(static_cast<Hemera::Application::OperationResult>(result), QString::fromStdString(errorName), QString::fromStdString(errorMessage));
}

void Application::setPreInitFunction(_fn_impl pre_init, void *data)
{
    d->pre_init = pre_init;
    d->pre_init_data = data;
}

void Application::setPreStartFunction(_fn_impl pre_start, void *data)
{
    d->pre_start = pre_start;
    d->pre_start_data = data;
}

void Application::setPreStopFunction(_fn_impl pre_stop, void *data)
{
    d->pre_stop = pre_stop;
    d->pre_stop_data = data;
}

void Application::setPreShutdownFunction(_fn_impl pre_shutdown, void *data)
{
    d->pre_shutdown = pre_shutdown;
    d->pre_shutdown_data = data;
}

void Application::setInitFunction(_fn_impl init, void *data)
{
    d->init = init;
    d->init_data = data;
}

void Application::setStartFunction(_fn_impl start, void *data)
{
    d->start = start;
    d->start_data = data;
}

void Application::setStopFunction(_fn_impl stop, void *data)
{
    d->stop = stop;
    d->stop_data = data;
}

void Application::setShutdownFunction(_fn_impl shutdown, void *data)
{
    d->shutdown = shutdown;
    d->shutdown_data = data;
}

void Application::initImpl()
{
    // Pre init is handled correctly by the hook in the proxy application.
    if (d->init != nullptr) {
        d->init(d->init_data);
    } else {
        setReady();
    }
}

void Application::startImpl()
{
    if (d->pre_start != nullptr) {
        d->pre_start(d->pre_start_data);
    }

    if (d->start != nullptr) {
        d->start(d->start_data);
    } else {
        setStarted();
    }
}

void Application::stopImpl()
{
    if (d->pre_stop != nullptr) {
        d->pre_stop(d->pre_stop_data);
    }

    if (d->stop != nullptr) {
        d->stop(d->stop_data);
    } else {
        setStopped();
    }
}

void Application::prepareForShutdown()
{
    if (d->pre_shutdown != nullptr) {
        d->pre_shutdown(d->pre_shutdown_data);
    }

    if (d->shutdown != nullptr) {
        d->shutdown(d->shutdown_data);
    } else {
        setReadyForShutdown();
    }
}

void Application::init()
{
    d->proxy->init();
}

}
