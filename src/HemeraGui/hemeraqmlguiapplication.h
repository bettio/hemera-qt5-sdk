/*
 *
 */

#ifndef HEMERA_QMLGUIAPPLICATION_H
#define HEMERA_QMLGUIAPPLICATION_H

#include <QtCore/QUrl>

#include <HemeraGui/GuiApplication>

class QQmlContext;
class QQmlEngine;
class QQuickItem;
class QQuickView;

namespace Hemera {

class QmlGuiApplicationPrivate;
/**
 * @class QmlGuiApplication
 * @ingroup HemeraGui
 * @headerfile HemeraGui/hemeraqmlguiapplication.h <HemeraGui/QmlGuiApplication>
 *
 * @brief The base class for Hemera Qml Gui applications
 *
 * QmlGuiApplication holds all the needed logic for applications based upon Qt Quick 2.
 *
 * Compared to GuiApplication, it does not manage QWindow directly, but only takes a QUrl
 * of a Qml file instead. Such a url can be also the URL of a resource or qrc resource.
 *
 * QmlGuiApplication should be used when you need to mix Qml and C++ logic. If you don't need
 * C++ logic in your application, you can simply use SimpleQmlGuiApplication in your ha file,
 * which does not require you to write any C++ code.
 *
 * During the initialization phase, it is possible to interact with the QML engine before
 * any component is loaded. This allows to interact, for example, with the context
 * before it starts taking effect.
 *
 * A typical usage of QmlGuiApplication is:
 *
 * @code
 * void MyQmlGuiApplication::initImpl()
 * {
 *     qmlEngine()->rootContext()->setContextProperty(QStringLiteral("myProperty"), this);
 *     setReady();
 * }
 * @endcode
 *
 * @note QmlGuiApplication requires your device to support a working 3D stack. Please enquire
 *       with your representative to find out if that is the case. Running a QmlGuiApplication
 *       on an unsupported device is, besides unsupported, likely not to be working.
 */
class QmlGuiApplication : public Hemera::GuiApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(QmlGuiApplication)
    Q_DECLARE_PRIVATE_D(d_h_ptr, QmlGuiApplication)

public:
    /**
     * Default constructor.
     *
     * @p mainQmlFile The main Qml file to be loaded on startup. Usually a resource or qrc path.
     */
    explicit QmlGuiApplication(ApplicationProperties *properties, const QUrl &mainQmlFile = QUrl());
    /**
     * Default destructor.
     */
    virtual ~QmlGuiApplication();

protected:
    explicit QmlGuiApplication(QmlGuiApplicationPrivate &dd, ApplicationProperties *properties, const QUrl &mainQmlFile = QUrl());

    /**
     * @returns The root Qml context
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QQmlContext *rootContext();

    /**
     * @returns The Qml root object
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QQuickItem *rootObject();
    /**
     * @returns The Qml engine for this application
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QQmlEngine *qmlEngine();

    /**
     * @returns The main QQuickView for this application
     *
     * @note This function returns a meaningful value only when the initialization procedure is started.
     *       It can be called in initImpl, but MUST NOT be called in the constructor.
     */
    QQuickView *mainView();

    /**
     * Set main QML source file that will be displayed
     *
     * @p mainQmlFile The main Qml file to be loaded. Usually a resource or qrc path.
     */
    void setMainQmlFile(const QUrl &mainQmlFile);
};

}

#endif // HEMERA_QMLGUIAPPLICATION_H
