#include "hemeraqmlsoftwaremanagementplugin.h"

#include <QtQml/QQmlComponent>

#include <HemeraSoftwareManagement/SystemUpdate>

// Components
#include "hemeraqmlsoftwaremanagementappliancemanager.h"
#include "hemeraqmlsoftwaremanagementprogressreporter.h"
#include "hemeraqmlsoftwaremanagementsystemupdate.h"

#include <hemeractconfig.h>

namespace Hemera
{

namespace Qml
{

namespace SoftwareManagement
{

void SoftwareManagementPlugin::registerTypes(const char* uri)
{
    int versionMajor = StaticConfig::qt5SDKMajorVersion();
    int versionMinor = StaticConfig::qt5SDKMinorVersion();

    qmlRegisterType<ApplianceManager>(uri, versionMajor, versionMinor, "ApplianceManager");
    qmlRegisterType<ProgressReporter>(uri, versionMajor, versionMinor, "ProgressReporter");

    // Enums
    qmlRegisterUncreatableType<Hemera::SoftwareManagement::SystemUpdate>(uri, versionMajor, versionMinor, "SystemUpdateType",
                                                                         QStringLiteral("SystemUpdateType is an enum, it cannot be created directly."));

    // Interfaces
    qmlRegisterUncreatableType<SystemUpdate>(uri, versionMajor, versionMinor, "SystemUpdate",
                                             QStringLiteral("SystemUpdate cannot be created directly."));

    // Metatypes
    qRegisterMetaType<Hemera::SoftwareManagement::SystemUpdate::UpdateType>();
}

void SoftwareManagementPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}

}

}

}
