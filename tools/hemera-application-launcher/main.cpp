#include "hemeraapplicationlauncher.h"

#include "applicationlauncherproperties.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include <iostream>

Q_LOGGING_CATEGORY(LOG_MAIN_APPLICATION_LAUNCHER, "Hemera Application Launcher")

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList arguments = app.arguments();

    // Usage: hemera-application-launcher /path/to/my.app.ha executable [args...]
    if (arguments.count() < 3) {
        return -1;
    }

    ApplicationLauncherProperties *properties = new ApplicationLauncherProperties(arguments.at(1));
    HemeraApplicationLauncher *launcher = nullptr;

    QObject::connect(properties, &ApplicationLauncherProperties::completed, [&launcher, properties, arguments] (bool success) {
        if (success) {
            qCDebug(LOG_MAIN_APPLICATION_LAUNCHER) << HemeraApplicationLauncher::tr("Starting application proxy");

            launcher = new HemeraApplicationLauncher(properties, arguments.at(2), [arguments] () -> QStringList { if (arguments.count() >= 4) return arguments.mid(3); else return QStringList(); }());
            launcher->init();
        }
    });

    Hemera::Operation *op = properties->init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << HemeraApplicationLauncher::tr("PropertiesGenerator could not be initialized. %1: %2")
                         .arg(op->errorName(), op->errorMessage()).toStdString() << std::endl;
            QCoreApplication::instance()->exit(EXIT_FAILURE);
        }
    });

    int retVal = app.exec();

    if (launcher != nullptr) {
        delete launcher;
    }

    return retVal;
}
