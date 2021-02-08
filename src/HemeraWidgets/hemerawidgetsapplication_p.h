#ifndef HEMERA_HEMERAWIDGETSAPPLICATION_P_H
#define HEMERA_HEMERAWIDGETSAPPLICATION_P_H

#include <HemeraWidgets/WidgetsApplication>

#include <private/HemeraCore/hemeraapplication_p.h>

namespace Hemera
{

class WidgetsApplicationPrivate : public ApplicationPrivate
{
public:
    WidgetsApplicationPrivate(WidgetsApplication *q) : ApplicationPrivate(q), mainWindow(nullptr) {}
    virtual ~WidgetsApplicationPrivate() {}

    Q_DECLARE_PUBLIC(WidgetsApplication)

    virtual void initHook() Q_DECL_OVERRIDE;

    QWidget *mainWindow;
};

}

#endif
