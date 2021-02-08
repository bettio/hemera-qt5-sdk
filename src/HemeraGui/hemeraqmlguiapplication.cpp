/*
 *
 */

#include "hemeraqmlguiapplication_p.h"

#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include <HemeraCore/Application>
#include <HemeraQml/QmlEngine>

namespace Hemera {

void QmlGuiApplicationPrivate::initHook()
{
    mainQuickView = new QQuickView(Hemera::qmlEngine(), nullptr);
    // avoid that ugly white flash that is displayed during application startup
    // we try to avoid clear before rendering until the QML source is loaded
    mainQuickView->setColor(Qt::black);
    mainQuickView->setClearBeforeRendering(false);

    mainQuickView = new QQuickView(Hemera::qmlEngine(), nullptr);

    Q_Q(QmlGuiApplication);
    q->setMainWindow(mainQuickView);

    mainQuickView->setResizeMode(QQuickView::SizeRootObjectToView);

    mainQuickView->engine()->rootContext()->setContextProperty(QStringLiteral("HemeraApplication"), Application::instance());

    Hemera::GuiApplicationPrivate::initHook();
}

void QmlGuiApplicationPrivate::postInitHook()
{
    mainQuickView->setSource(mainQmlFile);
    mainQuickView->setClearBeforeRendering(true);

    Hemera::GuiApplicationPrivate::postInitHook();
}


QmlGuiApplication::QmlGuiApplication(ApplicationProperties *properties, const QUrl &mainQmlFile)
    : GuiApplication(*new QmlGuiApplicationPrivate(this), properties)
{
    Q_D(QmlGuiApplication);
    d->mainQmlFile = mainQmlFile;
}

QmlGuiApplication::QmlGuiApplication(QmlGuiApplicationPrivate &dd, ApplicationProperties *properties, const QUrl &mainQmlFile)
    : GuiApplication(dd, properties)
{
    Q_D(QmlGuiApplication);
    d->mainQmlFile = mainQmlFile;
}

QmlGuiApplication::~QmlGuiApplication()
{
}

QQmlEngine *QmlGuiApplication::qmlEngine()
{
    Q_D(QmlGuiApplication);
    return d->mainQuickView->engine();
}

QQmlContext *QmlGuiApplication::rootContext()
{
    Q_D(QmlGuiApplication);
    return d->mainQuickView->rootContext();
}

QQuickItem *QmlGuiApplication::rootObject()
{
    Q_D(QmlGuiApplication);
    return d->mainQuickView->rootObject();
}

QQuickView *QmlGuiApplication::mainView()
{
    Q_D(QmlGuiApplication);
    return d->mainQuickView;
}

void QmlGuiApplication::setMainQmlFile(const QUrl &mainQmlFile)
{
    Q_D(QmlGuiApplication);

    d->mainQmlFile = mainQmlFile;

    if (d->mainQuickView && (mainQmlFile != d->mainQmlFile)) {
        d->mainQuickView->setSource(mainQmlFile);
    }
}

}
