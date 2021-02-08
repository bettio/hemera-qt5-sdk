/*
 *
 */

#include "hemerasimpleqmlguiapplication.h"

#include "hemeraqmlguiapplication_p.h"

namespace Hemera {

class SimpleQmlGuiApplicationPrivate : public QmlGuiApplicationPrivate
{
public:
    SimpleQmlGuiApplicationPrivate(SimpleQmlGuiApplication *q) : QmlGuiApplicationPrivate(q) {}
    virtual ~SimpleQmlGuiApplicationPrivate() {}

    Q_DECLARE_PUBLIC(SimpleQmlGuiApplication)

    virtual void initHook() Q_DECL_OVERRIDE Q_DECL_FINAL;

    QUrl mainQmlFile;
};

void SimpleQmlGuiApplicationPrivate::initHook()
{
    Hemera::QmlGuiApplicationPrivate::initHook();
}

SimpleQmlGuiApplication::SimpleQmlGuiApplication(ApplicationProperties *properties, const QUrl &mainQmlFile)
    : QmlGuiApplication(*new SimpleQmlGuiApplicationPrivate(this), properties, mainQmlFile)
{
}

SimpleQmlGuiApplication::~SimpleQmlGuiApplication()
{
}

void SimpleQmlGuiApplication::initImpl()
{
    setReady();
}

void SimpleQmlGuiApplication::startImpl()
{
    setStarted();
}

void SimpleQmlGuiApplication::stopImpl()
{
    setStopped();
}

void SimpleQmlGuiApplication::prepareForShutdown()
{
    Hemera::Application::prepareForShutdown();
}

}
