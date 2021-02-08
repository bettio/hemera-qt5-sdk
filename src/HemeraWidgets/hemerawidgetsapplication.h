#ifndef HEMERA_HEMERAWIDGETSAPPLICATION_H
#define HEMERA_HEMERAWIDGETSAPPLICATION_H

#include <HemeraCore/Application>

#include <QtWidgets/QApplication>

class QWindow;
class QWidget;

/**
 * @defgroup HemeraWidgets Hemera Widgets
 *
 * Hemera Widgets is the backbone for non-GL Hemera Applications. As a direct dependency to
 * @ref HemeraGui "Hemera GUI", it extends it to support non-GL applications. It should be used
 * on those devices where OpenGL is not supported.
 *
 * It is contained in the Hemera:: namespace.
 */

namespace Hemera {

class WidgetsApplicationPrivate;
/**
 * @class WidgetsApplication
 * @ingroup HemeraWidgets
 * @headerfile HemeraWidgets/hemerawidgetsapplication.h <HemeraGui/WidgetsApplication>
 *
 * @brief The base class for Hemera Widgets applications
 *
 * WidgetsApplication holds all the needed logic for applications based upon Qt Quick 1 or QtWidgets.
 *
 * As a counterpart to GuiApplication, it allows to set a main Widget which will then be managed
 * by the application as its main window. If you are relying
 * on Qt Quick1/QML, you might be better off sublcassing @ref Hemera::QtQuick1Application or
 * @ref Hemera::SimpleQtQuick1Application.
 *
 * @note WidgetsApplication uses a software rasterizer to paint on the screen. In case your device
 *       is GL capable, you might be better off using @ref Hemera::QmlGuiApplication. Enquire
 *       with your representative to find out if that is the case. On the other hand, running
 *       a WidgetsApplication is supported on every Hemera device, even if it does not guarantee
 *       the best performance.
 *
 * @note Please note that running Widgets-based applications on EGLFS, although possibly working,
 *       it is not supported, and you are voiding your warranty when doing so. The advised choice is
 *       either LinuxFB, DirectFB or X11. Please enquire with your Hemera representative to learn more.
 */
class HEMERA_QT5_SDK_EXPORT WidgetsApplication : public Hemera::Application
{
    Q_OBJECT
    Q_DISABLE_COPY(WidgetsApplication)
    Q_DECLARE_PRIVATE_D(d_h_ptr, WidgetsApplication)

public:
    /// Default constructor
    explicit WidgetsApplication(ApplicationProperties *properties);
    /// Default destructor
    virtual ~WidgetsApplication();

public Q_SLOTS:
    /**
     * @brief Sets the main widget for this application
     *
     * WidgetsApplication manages the state of your application's window visibility and more. As such,
     * you should provide your QWidget to be managed. You won't need and must not call show or any
     * similar modifier method on your window, and instead let WidgetsApplication manage it.
     *
     * @note Usually, setMainWidget should be called during the initialization procedure of the
     *       application.
     *
     * @note You should not use this function if you are using a higher level implementation of WidgetsApplication
     *       such as QtQuick1Application. Use the higher level alternatives instead.
     *
     * @note Do not attempt to call Hemera::GuiApplication's overload of setMainWindow!
     *
     * @sa Hemera::GuiApplication
     */
    void setMainWindow(QWidget *window);

protected:
    explicit WidgetsApplication(WidgetsApplicationPrivate &dd, ApplicationProperties *properties);
};

}

#endif // HEMERA_HEMERAWIDGETSAPPLICATION_H
