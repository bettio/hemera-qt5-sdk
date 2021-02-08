/*
 *
 */

#include "hemerasoftwaremanagementappliancemanager.h"
#include "hemerasoftwaremanagementconstructors_p.h"

#include "hemeraasyncinitobject_p.h"

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

#include "appliancemanagerinterface.h"
#include "dbusobjectinterface.h"

Q_LOGGING_CATEGORY(LOG_HEMERA_APPLIANCEMANAGER, "Hemera::SoftwareManagement::ApplianceManager")

namespace Hemera {

namespace SoftwareManagement {

// DBus timeout, 24 hours.
constexpr int dbusTimeout() { return 1000 * 60 * 60 * 24; }

class ApplianceManagerPrivate : public AsyncInitObjectPrivate
{
public:
    ApplianceManagerPrivate(ApplianceManager *q) : AsyncInitObjectPrivate(q) {}
    virtual ~ApplianceManagerPrivate() {}

    Q_DECLARE_PUBLIC(ApplianceManager)

    virtual void initHook() Q_DECL_OVERRIDE;

    com::ispirata::Hemera::SoftwareManagement::ApplianceManager *applianceManagerInterface;
    com::ispirata::Hemera::DBusObject *applianceManagerObjectInterface;

    ApplicationUpdates applicationUpdates;
    SystemUpdate systemUpdate;
    ApplicationPackages installedApplications;
    QDateTime lastCheckForUpdates;

    QString applianceName;
    QString applianceVariant;
    QString applianceVersion;
    QString hemeraVersion;

    void cacheVersioningInformation();
};

void ApplianceManagerPrivate::initHook()
{
    Hemera::AsyncInitObjectPrivate::initHook();
}

void ApplianceManagerPrivate::cacheVersioningInformation()
{
    Q_Q(ApplianceManager);

    QFile f(QStringLiteral("/etc/hemera_version"));
    f.open(QIODevice::ReadOnly | QIODevice::Text);

    QString hV = QString::fromLatin1(f.readAll());
    hV = hV.remove(QLatin1Char('\n'));

    bool notifyHV = false;
    if (!hemeraVersion.isEmpty() && hemeraVersion != hV) {
        notifyHV = true;
    }

    hemeraVersion = hV;

    if (notifyHV) {
        Q_EMIT q->hemeraVersionChanged();
    }

    // Appliance
    QSettings applianceData(QStringLiteral("/etc/hemera/appliance_manifest"), QSettings::IniFormat);
    applianceName = applianceData.value(QStringLiteral("APPLIANCE_NAME")).toString();
    applianceVariant = applianceData.value(QStringLiteral("APPLIANCE_VARIANT")).toString();

    bool notifyAV = false;
    if (!applianceVersion.isEmpty() && applianceVersion != applianceData.value(QStringLiteral("APPLIANCE_VERSION")).toString()) {
        notifyAV = true;
    }

    applianceVersion = applianceData.value(QStringLiteral("APPLIANCE_VERSION")).toString();

    if (notifyAV) {
        Q_EMIT q->applianceVersionChanged();
    }
}


ApplianceManager::ApplianceManager(QObject* parent)
    : AsyncInitObject(*new ApplianceManagerPrivate(this), parent)
{
}

ApplianceManager::~ApplianceManager()
{
}

void ApplianceManager::initImpl()
{
    // First of all, we want to check whether our service is up.
    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(Literals::literal(Literals::DBus::applianceManagerService()))) {
        // Houston, we have a problem
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("There's no software manager on this Hemera system, or the software manager failed on startup."));
        return;
    }

    Q_D(ApplianceManager);

    d->applianceManagerInterface = new com::ispirata::Hemera::SoftwareManagement::ApplianceManager(
                                                            Literals::literal(Literals::DBus::applianceManagerService()),
                                                            Literals::literal(Literals::DBus::applianceManagerPath()),
                                                            QDBusConnection::systemBus(), this);

    if (!d->applianceManagerInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus interface is not available. The software manager has probably encountered a failure."));
        return;
    }

    d->applianceManagerObjectInterface = new com::ispirata::Hemera::DBusObject(Literals::literal(Literals::DBus::applianceManagerService()),
                                                                      Literals::literal(Literals::DBus::applianceManagerPath()),
                                                                      QDBusConnection::systemBus(), this);

    if (!d->applianceManagerObjectInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus object interface is not available. The software manager has probably encountered a failure."));
        return;
    }

    // Set a high enough timeout (24 hours), as we about to place loooong calls.
    d->applianceManagerInterface->setTimeout(dbusTimeout());
    d->applianceManagerObjectInterface->setTimeout(dbusTimeout());

    setParts(2);

    // Cache properties and connect to signals
    DBusVariantMapOperation *operation = new DBusVariantMapOperation(d->applianceManagerObjectInterface->allProperties(), this);
    connect(operation, &Operation::finished, this, [this, d, operation] {
        if (operation->isError()) {
            setInitError(operation->errorName(), operation->errorMessage());
            return;
        }

        // Set the various properties
        QVariantMap result = operation->result();
        if (!result.value(QStringLiteral("initError")).toString().isEmpty()) {
            // The software manager has failed.
            setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                         tr("The software manager encountered an error: %1, %2").arg(result.value(QStringLiteral("initError")).toString(),
                                                                                     result.value(QStringLiteral("initErrorMessage")).toString()));
            return;
        }

        QByteArray systemUpdateAsJson = result.value(QStringLiteral("systemUpdate")).toByteArray();
        d->lastCheckForUpdates = QDateTime::fromMSecsSinceEpoch(result.value(QStringLiteral("lastCheckForUpdates")).toLongLong());
        Q_EMIT lastCheckForUpdatesChanged(d->lastCheckForUpdates);

        // Convert json
        if (!systemUpdateAsJson.isEmpty()) {
            d->systemUpdate = Constructors::systemUpdateFromJson(QJsonDocument::fromJson(systemUpdateAsJson).object());
            Q_EMIT availableSystemUpdateChanged();
        }

        setOnePartIsReady();
    });

    connect(d->applianceManagerObjectInterface, &com::ispirata::Hemera::DBusObject::propertiesChanged, this, [this, d] (const QVariantMap &changed) {
        if (changed.contains(QStringLiteral("systemUpdate"))) {
            d->systemUpdate = Constructors::systemUpdateFromJson(QJsonDocument::fromJson(changed.value(QStringLiteral("systemUpdate")).toByteArray()).object());
            Q_EMIT availableSystemUpdateChanged();
        }

        if (changed.contains(QStringLiteral("lastCheckForUpdates"))) {
            d->lastCheckForUpdates = QDateTime::fromMSecsSinceEpoch(changed.value(QStringLiteral("lastCheckForUpdates")).toLongLong());
            Q_EMIT lastCheckForUpdatesChanged(d->lastCheckForUpdates);
        }
    });

    d->cacheVersioningInformation();

    setOnePartIsReady();
}

SystemUpdate ApplianceManager::availableSystemUpdate() const
{
    Q_D(const ApplianceManager);
    return d->systemUpdate;
}

QDateTime ApplianceManager::lastCheckForUpdates() const
{
    Q_D(const ApplianceManager);

    return d->lastCheckForUpdates;
}

QString ApplianceManager::applianceName() const
{
    Q_D(const ApplianceManager);
    return d->applianceName;
}

QString ApplianceManager::applianceVariant() const
{
    Q_D(const ApplianceManager);
    return d->applianceVariant;
}

QString ApplianceManager::applianceVersion() const
{
    Q_D(const ApplianceManager);
    return d->applianceVersion;
}

QString ApplianceManager::hemeraVersion() const
{
    Q_D(const ApplianceManager);
    return d->hemeraVersion;
}

Operation *ApplianceManager::checkForUpdates(SystemUpdate::UpdateType preferredUpdateType)
{
    Q_D(ApplianceManager);

    return new DBusVoidOperation(d->applianceManagerInterface->checkForUpdates((quint16)preferredUpdateType), this);
}

Operation* ApplianceManager::downloadSystemUpdate()
{
    Q_D(ApplianceManager);

    if (!d->systemUpdate.isValid()) {
        return nullptr;
    }

    return new DBusVoidOperation(d->applianceManagerInterface->downloadSystemUpdate(), this);
}

Operation* ApplianceManager::updateSystem()
{
    Q_D(ApplianceManager);

    if (!d->systemUpdate.isValid()) {
        return nullptr;
    }

    Hemera::Operation *op = new DBusVoidOperation(d->applianceManagerInterface->updateSystem(), this);
    connect(op, &Hemera::Operation::finished, this, [this, op, d] {
        if (!op->isError()) {
            d->cacheVersioningInformation();
        }
    });

    return op;
}

}

}
