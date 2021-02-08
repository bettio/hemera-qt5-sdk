#ifndef HEMERA_HEMERAGUIAPPLICATION_H
#define HEMERA_HEMERAGUIAPPLICATION_H

#include <HemeraCore/Application>

#include <QtGui/QGuiApplication>

/**
 * @defgroup HemeraGui Hemera GUI
 *
 * Hemera GUI is the backbone for Hemera Applications which need to paint on the screen. It includes
 * components both for GL capable and uncapable devices. Hemera GUI is extended by @ref HemeraQml "Hemera QML"
 * and @ref HemeraWidgets "Hemera Widgets" respectively for QtQuick 2 application support (GL capable devices)
 * and QtWidgets/QtQuick1 application support (GL uncapable devices).
 *
 * It is contained in the Hemera:: namespace.
 */

class QWindow;

namespace Hemera {

class GuiApplicationPrivate;
/**
 * @class GuiApplication
 * @ingroup HemeraGui
 * @headerfile HemeraGui/hemeraguiapplication.h <HemeraGui/GuiApplication>
 *
 * @brief The base class for Hemera Gui applications
 *
 * GuiApplication holds all the needed logic for applications based upon QWindow.
 * This includes all QtQuick 2 applications.
 *
 * It follows all of the paradigms of Application, and is meant to be subclassed when
 * you have a specific need to handle all of the logic from QWindow up. If you are relying
 * on Qt Quick/QML, you might be better off sublcassing QmlGuiApplication or SimpleQmlGuiApplication.
 *
 * @note GuiApplication requires your device to support a working 3D stack. Please enquire
 *       with your representative to find out if that is the case. Running a GuiApplication
 *       on an unsupported device is, besides unsupported, likely not to be working.
 */
class HEMERA_QT5_SDK_EXPORT GuiApplication : public Hemera::Application
{
    Q_OBJECT
    Q_DISABLE_COPY(GuiApplication)
    Q_DECLARE_PRIVATE_D(d_h_ptr, GuiApplication)

public:
    /**
     * Default constructor
     */
    explicit GuiApplication(ApplicationProperties *properties);
    /**
     * Default destructor
     */
    virtual ~GuiApplication();

public Q_SLOTS:
    /**
     * Sets the main window for this application
     *
     * GuiApplication manages the state of your application's window visibility and more. As such,
     * you should provide your Window to be managed. You won't need and must not call show or any
     * similar modifier method on your window, and instead let GuiApplication manage it.
     *
     * @note Usually, setMainWindow should be called during the initialization procedure of the
     *       application.
     *
     * @note You should not use this function if you are using a higher level implementation of GuiApplication
     *       such as QmlGuiApplication. Use the higher level alternatives instead.
     */
    void setMainWindow(QWindow *window);

protected:
    explicit GuiApplication(GuiApplicationPrivate &dd, ApplicationProperties *properties);
};

}

#endif // HEMERA_HEMERAGUIAPPLICATION_H
