#ifndef HEMERA_HEMERAGUIAPPLICATION_P_H
#define HEMERA_HEMERAGUIAPPLICATION_P_H

#include <HemeraGui/GuiApplication>

#include <private/HemeraCore/hemeraapplication_p.h>

namespace Hemera
{

class GuiApplicationPrivate : public ApplicationPrivate
{
public:
    GuiApplicationPrivate(GuiApplication *q) : ApplicationPrivate(q), mainWindow(nullptr) {}
    virtual ~GuiApplicationPrivate() {}

    Q_DECLARE_PUBLIC(GuiApplication)

    virtual void initHook() Q_DECL_OVERRIDE;

    QWindow *mainWindow;
};

}

#endif
