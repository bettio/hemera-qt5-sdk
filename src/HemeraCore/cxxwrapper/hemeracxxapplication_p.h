#ifndef HEMERA_CXX_HEMERAAPPLICATION_P_H
#define HEMERA_CXX_HEMERAAPPLICATION_P_H

#include <HemeraCxxCore/Application>

#include <QtCore/QHash>

namespace HemeraCxx {

class Application::Private
{
public:
    Private() : pre_init(nullptr), pre_start(nullptr), pre_stop(nullptr), pre_shutdown(nullptr),
                init(nullptr), start(nullptr), stop(nullptr), shutdown(nullptr), applicationStatusCbId(0) {}

    ProxyQt5Application *proxy;

    // Pre
    _fn_impl pre_init;
    void *pre_init_data;
    _fn_impl pre_start;
    void *pre_start_data;
    _fn_impl pre_stop;
    void *pre_stop_data;
    _fn_impl pre_shutdown;
    void *pre_shutdown_data;

    _fn_impl init;
    void *init_data;
    _fn_impl start;
    void *start_data;
    _fn_impl stop;
    void *stop_data;
    _fn_impl shutdown;
    void *shutdown_data;

    QHash< int, QPair< _application_status_changed_cb, void* > > applicationStatusCbs;
    int applicationStatusCbId;

    // Proxied slots
    void onApplicationStatusChanged(HemeraCxx::Application::ApplicationStatus status);
};

}

#endif // HEMERA_CXX_HEMERAAPPLICATION_H
