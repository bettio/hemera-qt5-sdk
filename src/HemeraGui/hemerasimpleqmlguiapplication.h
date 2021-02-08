/*
 *
 */

#ifndef HEMERA_SIMPLEQMLAPPLICATION_H
#define HEMERA_SIMPLEQMLAPPLICATION_H

#include <HemeraGui/QmlGuiApplication>

namespace Hemera {

class SimpleQmlGuiApplicationPrivate;
/**
 * @class SimpleQmlGuiApplication
 * @ingroup HemeraGui
 * @headerfile HemeraGui/hemerasimpleqmlguiapplication.h <HemeraGui/SimpleQmlGuiApplication>
 *
 * @brief A simple and fully managed Qt Quick 2 application
 *
 * SimpleQmlGuiApplication holds all the logic and lifecycle management for a Qt Quick 2 application.
 * It should not be used directly: it is usually generated from a .ha file automatically. If you need
 * C++ logic, you are probably better off with QmlGuiApplication instead.
 *
 * @note SimpleQmlGuiApplication requires your device to support a working 3D stack. Please enquire
 *       with your representative to find out if that is the case. Running a SimpleQmlGuiApplication
 *       on an unsupported device is, besides unsupported, likely not to be working.
 */
class SimpleQmlGuiApplication : public Hemera::QmlGuiApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(SimpleQmlGuiApplication)
    Q_DECLARE_PRIVATE_D(d_h_ptr, SimpleQmlGuiApplication)

public:
    explicit SimpleQmlGuiApplication(ApplicationProperties *properties, const QUrl &mainQmlFile);
    virtual ~SimpleQmlGuiApplication();

public Q_SLOTS:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void stopImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void prepareForShutdown() Q_DECL_OVERRIDE Q_DECL_FINAL;

};
}

#endif // HEMERA_SIMPLEQMLAPPLICATION_H
