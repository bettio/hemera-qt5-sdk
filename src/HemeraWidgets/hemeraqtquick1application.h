/*
 *
 */

#ifndef HEMERA_QTQUICK1APPLICATION_H
#define HEMERA_QTQUICK1APPLICATION_H

#include <HemeraWidgets/WidgetsApplication>

class QDeclarativeContext;
class QDeclarativeEngine;
class QDeclarativeItem;
class QDeclarativeView;

namespace Hemera {

class QtQuick1ApplicationPrivate;
/**
 * @class QtQuick1Application
 * @ingroup HemeraWidgets
 * @headerfile HemeraWidgets/hemeraqtquick1application.h <HemeraWidgets/QtQuick1Application>
 *
 * @brief The base class for Hemera Qt Quick 1 applications
 *
 * QtQuick1Application holds all the needed logic for applications based upon Qt Quick 1.
 *
 * Compared to WidgetsApplication, it does not manage QWidget directly, but only takes a QUrl
 * of a Qml file instead. Such a URL can be also the URL of a resource or qrc resource.
 *
 * QtQuick1Application should be used when you need to mix Qml and C++ logic. If you don't need
 * C++ logic in your application, you can simply use @ref Hemera::SimpleQtQuick1Application in your ha file,
 * which does not require you to write any C++ code.
 *
 * During the initialization phase, it is possible to interact with the declarative engine before
 * any component is loaded. This allows to interact, for example, with the context
 * before it starts taking effect.
 *
 * A typical usage of QtQuick1Application is:
 *
 * @code
 * void MyQtQuick1Application::initImpl()
 * {
 *     rootContext()->setContextProperty(QStringLiteral("myProperty"), this);
 *     setReady();
 * }
 * @endcode
 *
 * @note QtQuick1Application uses a software rasterizer to paint on the screen. In case your device
 *       is GL capable, you might be better off using @ref Hemera::QmlGuiApplication. Enquire
 *       with your representative to find out if that is the case. On the other hand, running
 *       a WidgetsApplication is supported on every Hemera device, even if it does not guarantee
 *       the best performance.
 *
 * @note Please note that running Widgets-based applications on EGLFS, although possibly working,
 *       it is not supported, and you are voiding your warranty when doing so. The advised choice is
 *       either LinuxFB or X11. Please enquire with your Hemera representative to learn more.
 */
class QtQuick1Application : public Hemera::WidgetsApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(QtQuick1Application)
    Q_DECLARE_PRIVATE_D(d_h_ptr, QtQuick1Application)

public:
    /**
     * Default constructor.
     *
     * @p mainQmlFile The main Qml file to be loaded on startup. Usually a resource or qrc path.
     */
    explicit QtQuick1Application(ApplicationProperties *properties, const QUrl &mainQmlFile);
    /// Default destructor
    virtual ~QtQuick1Application();

protected:
    explicit QtQuick1Application(QtQuick1ApplicationPrivate &dd, ApplicationProperties *properties, const QUrl &mainQmlFile);

    /**
     * @returns The root declarative context
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QDeclarativeContext *rootContext();
    /**
     * @returns The root declarative object
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QDeclarativeItem *rootObject();
    /**
     * @returns The declarative engine for this application
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QDeclarativeEngine *declarativeEngine();

    /**
     * @returns The main QDeclarativeView for this application
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QDeclarativeView *mainView();
};
}

#endif // HEMERA_QTQUICK1APPLICATION_H
