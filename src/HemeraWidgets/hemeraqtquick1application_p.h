#ifndef HEMERA_HEMERAQTQUICK1APPLICATION_P_H
#define HEMERA_HEMERAQTQUICK1APPLICATION_P_H

#include <HemeraWidgets/QtQuick1Application>

#include <private/HemeraWidgets/hemerawidgetsapplication_p.h>

#include <QtCore/QUrl>

class QDeclarativeView;

namespace Hemera
{

class QtQuick1ApplicationPrivate : public WidgetsApplicationPrivate
{
public:
    QtQuick1ApplicationPrivate(QtQuick1Application *q) : WidgetsApplicationPrivate(q) {}
    virtual ~QtQuick1ApplicationPrivate() {}

    Q_DECLARE_PUBLIC(QtQuick1Application)

    virtual void initHook() Q_DECL_OVERRIDE;
    virtual void postInitHook() Q_DECL_OVERRIDE;

    QUrl mainQmlFile;
    QDeclarativeView *mainDeclarativeView;
};

}

#endif
