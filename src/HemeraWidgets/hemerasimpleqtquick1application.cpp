/*
 *
 */

#include "hemerasimpleqtquick1application.h"

#include "hemeraqtquick1application_p.h"

namespace Hemera {

class SimpleQtQuick1ApplicationPrivate : public QtQuick1ApplicationPrivate
{
public:
    SimpleQtQuick1ApplicationPrivate(SimpleQtQuick1Application *q) : QtQuick1ApplicationPrivate(q) {}
    virtual ~SimpleQtQuick1ApplicationPrivate() {}

    Q_DECLARE_PUBLIC(SimpleQtQuick1Application)

    virtual void initHook() Q_DECL_OVERRIDE Q_DECL_FINAL;

    QUrl mainQmlFile;
};

void SimpleQtQuick1ApplicationPrivate::initHook()
{
    Hemera::QtQuick1ApplicationPrivate::initHook();
}

SimpleQtQuick1Application::SimpleQtQuick1Application(ApplicationProperties *properties, const QUrl &mainQmlFile)
    : QtQuick1Application(*new SimpleQtQuick1ApplicationPrivate(this), properties, mainQmlFile)
{
}

SimpleQtQuick1Application::~SimpleQtQuick1Application()
{
}

void SimpleQtQuick1Application::initImpl()
{
    setReady();
}

void SimpleQtQuick1Application::startImpl()
{
    setStarted();
}

void SimpleQtQuick1Application::stopImpl()
{
    setStopped();
}

void SimpleQtQuick1Application::prepareForShutdown()
{
    Hemera::Application::prepareForShutdown();
}

}
