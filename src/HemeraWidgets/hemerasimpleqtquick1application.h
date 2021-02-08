/*
 *
 */

#ifndef HEMERA_SIMPLEQTQUICK1APPLICATION_H
#define HEMERA_SIMPLEQTQUICK1APPLICATION_H

#include <HemeraWidgets/QtQuick1Application>

namespace Hemera {

class SimpleQtQuick1ApplicationPrivate;
/**
 * @class SimpleQtQuick1Application
 * @ingroup HemeraWidgets
 * @headerfile HemeraWidgets/hemerasimpleqmlguiapplication.h <HemeraWidgets/SimpleQtQuick1Application>
 *
 * @brief A simple and fully managed Qt Quick 1 application
 *
 * SimpleQtQuick1Application holds all the logic and lifecycle management for a Qt Quick 1 application.
 * It should not be used directly: it is usually generated from a .ha file automatically. If you need
 * C++ logic, you are probably better off with @ref Hemera::QtQuick1Application instead.
 *
 * @note SimpleQtQuick1Application uses a software rasterizer to paint on the screen. In case your device
 *       is GL capable, you might be better off using @ref Hemera::SimpleQmlGuiApplication. Enquire
 *       with your representative to find out if that is the case. On the other hand, running
 *       a WidgetsApplication is supported on every Hemera device, even if it does not guarantee
 *       the best performance.
 *
 * @note Please note that running Widgets-based applications on EGLFS, although possibly working,
 *       it is not supported, and you are voiding your warranty when doing so. The advised choice is
 *       either LinuxFB or X11. Please enquire with your Hemera representative to learn more.
 */
class SimpleQtQuick1Application : public Hemera::QtQuick1Application
{
    Q_OBJECT
    Q_DISABLE_COPY(SimpleQtQuick1Application)
    Q_DECLARE_PRIVATE_D(d_h_ptr, SimpleQtQuick1Application)

public:
    explicit SimpleQtQuick1Application(ApplicationProperties *properties, const QUrl &mainQmlFile);
    virtual ~SimpleQtQuick1Application();

public Q_SLOTS:
    virtual void initImpl() Q_DECL_OVERRIDE;
    virtual void startImpl() Q_DECL_OVERRIDE;
    virtual void stopImpl() Q_DECL_OVERRIDE;
    virtual void prepareForShutdown() Q_DECL_OVERRIDE;

};
}

#endif // HEMERA_SIMPLEQMLAPPLICATION_H
