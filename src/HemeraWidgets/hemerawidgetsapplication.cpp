#include "hemerawidgetsapplication_p.h"

#include <HemeraCore/ApplicationProperties>

#include <QtCore/QLoggingCategory>

#include <QtWidgets/QWidget>

Q_LOGGING_CATEGORY(LOG_HEMERA_WIDGETSAPPLICATION, "Hemera::WidgetsApplication")

namespace Hemera {

void WidgetsApplicationPrivate::initHook()
{
    QApplication *app = qobject_cast<QApplication*>(QApplication::instance());
    Q_ASSERT(app);
    app->setQuitOnLastWindowClosed(false);

    Hemera::ApplicationPrivate::initHook();
}

WidgetsApplication::WidgetsApplication(ApplicationProperties *properties)
    : Application(*new WidgetsApplicationPrivate(this), properties)
{
    connect(this, &Application::applicationStatusChanged, [this] (Hemera::Application::ApplicationStatus status) {
            Q_D(WidgetsApplication);
            Q_ASSERT(d->mainWindow);
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

WidgetsApplication::WidgetsApplication(WidgetsApplicationPrivate& dd, ApplicationProperties* properties)
    : Application(dd, properties)
{
    connect(this, &Application::applicationStatusChanged, [this] (Hemera::Application::ApplicationStatus status) {
        Q_D(WidgetsApplication);
        Q_ASSERT(d->mainWindow);
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

WidgetsApplication::~WidgetsApplication()
{
    Q_D(WidgetsApplication);

    if (d->mainWindow) {
        d->mainWindow->deleteLater();
    }
}

void WidgetsApplication::setMainWindow(QWidget *window)
{
    Q_D(WidgetsApplication);

    Q_ASSERT(applicationStatus() == ApplicationStatus::Initializing || applicationStatus() == ApplicationStatus::NotInitialized);

    if (d->mainWindow) {
        qCWarning(LOG_HEMERA_WIDGETSAPPLICATION) << tr("Trying to reset the main window!!!");
        return;
    }

    window->setWindowState(Qt::WindowFullScreen);

    d->mainWindow = window;
}

}
