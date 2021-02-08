/*
 *
 */

#include "hemeraqtquick1application_p.h"

#include "hemerawidgetsapplication_p.h"

#include <HemeraWidgets/DeclarativeEngine>

#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeView>

namespace Hemera
{

void QtQuick1ApplicationPrivate::initHook()
{
    mainDeclarativeView = new QDeclarativeView(nullptr);
    Hemera::processDeclarativeEngine(mainDeclarativeView->engine());

    Q_Q(QtQuick1Application);
    q->setMainWindow(mainDeclarativeView);
    mainDeclarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    Hemera::WidgetsApplicationPrivate::initHook();
}

void QtQuick1ApplicationPrivate::postInitHook()
{
    mainDeclarativeView->setSource(mainQmlFile);

    Hemera::WidgetsApplicationPrivate::postInitHook();
}

QtQuick1Application::QtQuick1Application(ApplicationProperties* properties, const QUrl& mainQmlFile)
    : WidgetsApplication(*new QtQuick1ApplicationPrivate(this), properties)
{
    Q_D(QtQuick1Application);
    d->mainQmlFile = mainQmlFile;
}

QtQuick1Application::QtQuick1Application(QtQuick1ApplicationPrivate &dd, ApplicationProperties *properties, const QUrl &mainQmlFile)
    : WidgetsApplication(dd, properties)
{
    Q_D(QtQuick1Application);
    d->mainQmlFile = mainQmlFile;
}

QtQuick1Application::~QtQuick1Application()
{

}

QDeclarativeEngine* QtQuick1Application::declarativeEngine()
{
    Q_D(QtQuick1Application);
    return d->mainDeclarativeView->engine();
}

QDeclarativeContext* QtQuick1Application::rootContext()
{
    Q_D(QtQuick1Application);
    return d->mainDeclarativeView->rootContext();
}

QDeclarativeItem* QtQuick1Application::rootObject()
{
    Q_D(QtQuick1Application);
    return qobject_cast<QDeclarativeItem*>(d->mainDeclarativeView->rootObject());
}

QDeclarativeView* QtQuick1Application::mainView()
{
    Q_D(QtQuick1Application);
    return d->mainDeclarativeView;
}

}
