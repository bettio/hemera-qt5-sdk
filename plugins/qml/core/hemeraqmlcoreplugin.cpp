#include "hemeraqmlcoreplugin.h"

#include <QtQml/QQmlComponent>

// Components
#include "hemeraoperation.h"
#include "hemeraoperationwithprogress.h"
#include "hemeraapplication.h"
#include "hemeracommonoperations.h"
#include "hemeradevicemanagement.h"
#include "hemeraservicemodel.h"

#include <hemeractconfig.h>

namespace Hemera
{

namespace Qml
{

namespace Core
{

void CorePlugin::registerTypes(const char* uri)
{
    int versionMajor = StaticConfig::qt5SDKMajorVersion();
    int versionMinor = StaticConfig::qt5SDKMinorVersion();

    qmlRegisterType<ServiceModel>(uri, versionMajor, versionMinor, "HemeraServiceModel");

    // Interfaces
    qmlRegisterUncreatableType<DeviceManagement>(uri, versionMajor, versionMinor, "DeviceManagement",
                                                 QStringLiteral("DeviceManagement is a static class, and cannot be created directly."));
    qmlRegisterUncreatableType<Operation>(uri, versionMajor, versionMinor, "HemeraOperation",
                                          QStringLiteral("Operation is an interface, and cannot be created directly."));
    qmlRegisterUncreatableType<OperationWithProgress>(uri, versionMajor, versionMinor, "HemeraOperationWithProgress",
                                                      QStringLiteral("OperationWithProgress is an interface, and cannot be created directly."));
    qmlRegisterUncreatableType<Application>(uri, versionMajor, versionMinor, "HemeraApplication",
                                            QStringLiteral("Application is an interface, and cannot be created directly."));
}

void CorePlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}

}

}

}
