#ifndef HEMERA_HEMERALITERALS_H
#define HEMERA_HEMERALITERALS_H

#include <QtCore/QLatin1String>

namespace Hemera {

namespace Literals {

static constexpr QLatin1String literal(const char *l) { return QLatin1String(l); }

/// DBus interfaces, services and paths
namespace DBus {

constexpr QLatin1String starBus() { return QLatin1String("starbus"); }

constexpr const char *applianceCryptoInterface() { return "com.ispirata.Hemera.ApplianceCrypto"; }
constexpr const char *applianceCryptoPath() { return "/com/ispirata/Hemera/ApplianceCrypto"; }
constexpr const char *applianceManagerService() { return "com.ispirata.Hemera.SoftwareManagement.ApplianceManager"; }
constexpr const char *applianceManagerInterface() { return "com.ispirata.Hemera.SoftwareManagement.ApplianceManager"; }
constexpr const char *applianceManagerPath() { return "/com/ispirata/Hemera/SoftwareManagement/ApplianceManager"; }
constexpr const char *applicationInterface() { return "com.ispirata.Hemera.Application"; }
constexpr const char *applicationPath() { return "/com/ispirata/Hemera/Application"; }
constexpr const char *applicationHandlerPath() { return "/com/ispirata/Hemera/Parsec/ApplicationHandler"; }
constexpr const char *applicationManagerService() { return "com.ispirata.Hemera.SoftwareManagement.ApplicationManager"; }
constexpr const char *applicationManagerInterface() { return "com.ispirata.Hemera.SoftwareManagement.ApplicationManager"; }
constexpr const char *applicationManagerPath() { return "/com/ispirata/Hemera/SoftwareManagement/ApplicationManager"; }
constexpr const char *dBusHolderInterface() { return "com.ispirata.Hemera.Application.DBusHolder"; }
constexpr const char *dBusHolderPath() { return "/com/ispirata/Hemera/Application/DBusHolder"; }
constexpr const char *deviceManagementInterface() { return "com.ispirata.Hemera.DeviceManagement"; }
constexpr const char *deviceManagementPath() { return "/com/ispirata/Hemera/DeviceManagement"; }
constexpr const char *developerModeService() { return "com.ispirata.Hemera.DeveloperMode"; }
constexpr const char *developerModePath() { return "/com/ispirata/Hemera/DeveloperMode"; }
constexpr const char *dbusObjectInterface() { return "com.ispirata.Hemera.DBusObject"; }
constexpr const char *dbusObjectPath() { return "/com/ispirata/Hemera/DBusObject"; }
constexpr const char *fingerprintsInterface() { return "com.ispirata.Hemera.Fingerprints"; }
constexpr const char *fingerprintsPath() { return "/com/ispirata/Hemera/Fingerprints"; }
constexpr const char *fingerprintsService() { return "com.ispirata.Hemera.Fingerprints"; }
constexpr const char *gravityGalaxyPath() { return "/com/ispirata/Hemera/Gravity/Galaxy"; }
constexpr const char *gravityGalaxyManagerPath() { return "/com/ispirata/Hemera/Gravity/GalaxyManager"; }
constexpr const char *gravityStarPath() { return "/com/ispirata/Hemera/Gravity/Galaxy/Star"; }
constexpr const char *gravityCenterService() { return "com.ispirata.Hemera.GravityCenter"; }
constexpr const char *gravityCenterPath() { return "/com/ispirata/Hemera/GravityCenter"; }
constexpr const char *planetInterface() { return "com.ispirata.Hemera.Planet"; }
constexpr const char *planetService() { return "com.ispirata.Hemera.Planet"; }
constexpr const char *planetPath() { return "/com/ispirata/Hemera/Planet"; }
constexpr const char *parsecInterface() { return "com.ispirata.Hemera.Parsec"; }
constexpr const char *parsecService() { return "com.ispirata.Hemera.Parsec"; }
constexpr const char *parsecPath() { return "/com/ispirata/Hemera/Parsec"; }
constexpr const char *removableStorageManagerInterface() { return "com.ispirata.Hemera.RemovableStorageManager"; }
constexpr const char *removableStorageManagerPath() { return "/com/ispirata/Hemera/RemovableStorageManager"; }
constexpr const char *satelliteManagerInterface() { return "com.ispirata.Hemera.Gravity.SatelliteManager"; }
constexpr const char *satelliteManagerPath() { return "/com/ispirata/Hemera/Gravity/Galaxy/Star/%1/SatelliteManager"; }
constexpr const char *softwareManagementProgressReporterInterface() { return "com.ispirata.Hemera.SoftwareManagement.ProgressReporter"; }
constexpr const char *softwareManagementProgressReporterPath() { return "/com/ispirata/Hemera/SoftwareManagement/ProgressReporter"; }
constexpr const char *softwareManagerService() { return "com.ispirata.Hemera.SoftwareManager"; }
constexpr const char *starSequenceInterface() { return "com.ispirata.Hemera.Gravity.StarSequence"; }
constexpr const char *starSequencePath() { return "/com/ispirata/Hemera/Gravity/Galaxy/Star/%1/StarSequence"; }
constexpr const char *usbGadgetManagerService() { return "com.ispirata.Hemera.USBGadgetManager"; }
constexpr const char *usbGadgetManagerInterface() { return "com.ispirata.Hemera.USBGadgetManager"; }
constexpr const char *usbGadgetManagerPath() { return "/com/ispirata/Hemera/USBGadgetManager"; }

}

/// Errors
namespace Errors {

constexpr const char *applicationStartFailed() { return "com.ispirata.Hemera.Error.ApplicationStartFailed"; }
constexpr const char *badRequest() { return "com.ispirata.Hemera.Error.BadRequest"; }
constexpr const char *canceled() { return "com.ispirata.Hemera.Error.Canceled"; }
constexpr const char *declarativeError() { return "com.ispirata.Hemera.Error.DeclarativeError"; }
constexpr const char *dbusObjectNotAvailable() { return "com.ispirata.Hemera.DBusObject.Error.InterfaceNotAvailable"; }
constexpr const char *errorHandlingError() { return "com.ispirata.Hemera.ErrorHandlingError"; }
constexpr const char *failedRequest() { return "com.ispirata.Hemera.Error.FailedRequest"; }
constexpr const char *interfaceNotAvailable() { return "com.ispirata.Hemera.Error.InterfaceNotAvailable"; }
constexpr const char *notAllowed() { return "com.ispirata.Hemera.Error.NotAllowed"; }
constexpr const char *notFound() { return "com.ispirata.Hemera.Error.NotFound"; }
constexpr const char *parseError() { return "com.ispirata.Hemera.Error.ParseError"; }
constexpr const char *registerObjectFailed() { return "com.ispirata.Hemera.Error.RegisterObjectFailed"; }
constexpr const char *registerServiceFailed() { return "com.ispirata.Hemera.Error.RegisterServiceFailed"; }
constexpr const char *systemdError() { return "com.ispirata.Hemera.Error.Systemd"; }
constexpr const char *timeout() { return "com.ispirata.Hemera.Error.Timeout"; }
constexpr const char *unhandledRequest() { return "com.ispirata.Hemera.Error.UnhandledRequest"; }
constexpr const char *wrongOrbitChange() { return "com.ispirata.Hemera.Gravity.OrbitHandler.Error.WrongOrbitChange"; }

}

}

}

#endif
