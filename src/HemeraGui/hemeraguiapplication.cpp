#include "hemeraguiapplication_p.h"

#include <HemeraCore/ApplicationProperties>

#include <QtCore/QLoggingCategory>

#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>

Q_LOGGING_CATEGORY(LOG_HEMERA_GUIAPPLICATION, "Hemera::GuiApplication")

namespace Hemera {

void GuiApplicationPrivate::initHook()
{
    QGuiApplication *app = qobject_cast<QGuiApplication*>(QGuiApplication::instance());
    Q_ASSERT(app);
    app->setQuitOnLastWindowClosed(false);

    Hemera::ApplicationPrivate::initHook();
}

GuiApplication::GuiApplication(ApplicationProperties *properties)
    : Application(*new GuiApplicationPrivate(this), properties)
{
    connect(this, &Application::applicationStatusChanged, [this] (Hemera::Application::ApplicationStatus status) {
            Q_D(GuiApplication);
            switch (status) {
                case ApplicationStatus::Running:
                    if (!isSatellite()) {
                        d->mainWindow->showFullScreen();
                    } else {
                        d->mainWindow->show();
                    }
                    break;
                case ApplicationStatus::Stopping:
                    d->mainWindow->hide();
                    break;
                default:
                    break;
            }
    });
}

GuiApplication::GuiApplication(GuiApplicationPrivate& dd, ApplicationProperties* properties)
    : Application(dd, properties)
{
    connect(this, &Application::applicationStatusChanged, [this] (Hemera::Application::ApplicationStatus status) {
        Q_D(GuiApplication);
        switch (status) {
            case ApplicationStatus::Running:
                if (!isSatellite()) {
                    d->mainWindow->showFullScreen();
                } else {
                    d->mainWindow->show();
                }
                break;
            case ApplicationStatus::Stopping:
                d->mainWindow->hide();
                break;
            default:
                break;
        }
    });
}

GuiApplication::~GuiApplication()
{
    Q_D(GuiApplication);

    if (d->mainWindow) {
        d->mainWindow->deleteLater();
    }
}

void GuiApplication::setMainWindow(QWindow *window)
{
    Q_D(GuiApplication);

    Q_ASSERT(applicationStatus() == ApplicationStatus::Initializing || applicationStatus() == ApplicationStatus::NotInitialized);

    if (d->mainWindow) {
        qCWarning(LOG_HEMERA_GUIAPPLICATION) << tr("Trying to reset the main window!!!");
        return;
    }

    window->setWindowState(Qt::WindowFullScreen);

    d->mainWindow = window;
}

}
