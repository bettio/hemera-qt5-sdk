#include "configure.h"

#include <HemeraCore/Literals>

#include <HemeraGenerators/BaseConfigure>
#include <HemeraGenerators/ConfigurePlugin>

#include <hemeractconfig.h>

#include <iostream>

Configure::Configure(const QString &haFile, const QString &destDir, QObject* parent)
    : BaseConfigure(haFile, destDir, true, parent)
{
    std::cout << "-- " << tr("Hemera Configure version %1").arg(QLatin1String(Hemera::StaticConfig::qt5SDKVersion())).toStdString() << std::endl;
    std::cout << "-- " << tr("Target preparation for %1").arg(haFile).toStdString() << std::endl;
}

Configure::~Configure()
{
}

void Configure::initImpl()
{
    // Let's see what to connect to:
    for (Hemera::Generators::ConfigurePlugin *plugin : plugins()) {
        if (plugin->handleClassName(haObject()->metaObject()->className())) {
            // It is done
            std::cout << "-- " << tr("Starting target configuration").toStdString() << std::endl;
            setReady();
            return;
        }
    }

    setInitError(Hemera::Literals::literal(Hemera::Literals::Errors::declarativeError()),
                 QStringLiteral("-- %1").arg(tr("ERROR: No compatible plugins were found for the given ha class, %1.").arg(QLatin1String(haObject()->metaObject()->className()))));
}
