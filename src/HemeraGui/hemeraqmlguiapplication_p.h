#ifndef HEMERA_HEMERAQMLGUIAPPLICATION_P_H
#define HEMERA_HEMERAQMLGUIAPPLICATION_P_H

#include <HemeraGui/QmlGuiApplication>

#include <private/HemeraGui/hemeraguiapplication_p.h>

#include <QtCore/QUrl>

class QQuickView;

namespace Hemera
{

class QmlGuiApplicationPrivate : public GuiApplicationPrivate
{
public:
    QmlGuiApplicationPrivate(QmlGuiApplication *q) : GuiApplicationPrivate(q) {}
    virtual ~QmlGuiApplicationPrivate() {}

    Q_DECLARE_PUBLIC(QmlGuiApplication)

    virtual void initHook() Q_DECL_OVERRIDE;
    virtual void postInitHook() Q_DECL_OVERRIDE;

    QUrl mainQmlFile;
    QQuickView *mainQuickView;
};

}

#endif
