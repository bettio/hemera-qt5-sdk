/*
 *
 */

#include "hemerasoftwaremanagementapplicationmanager.h"
#include "hemerasoftwaremanagementconstructors_p.h"

#include "hemeraasyncinitobject_p.h"

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

#include "applicationmanagerinterface.h"
#include "dbusobjectinterface.h"

Q_LOGGING_CATEGORY(LOG_HEMERA_APPLICATIONMANAGER, "Hemera::SoftwareManagement::ApplicationManager")

namespace Hemera {

namespace SoftwareManagement {

class ApplicationManagerPrivate : public AsyncInitObjectPrivate
{
public:
    ApplicationManagerPrivate(ApplicationManager *q) : AsyncInitObjectPrivate(q) {}
    virtual ~ApplicationManagerPrivate() {}

    Q_DECLARE_PUBLIC(ApplicationManager)

    virtual void initHook() Q_DECL_OVERRIDE;
    bool checkApplicationUpdatesCoherency(const ApplicationUpdates &updates);

    com::ispirata::Hemera::SoftwareManagement::ApplicationManager *applicationManagerInterface;
    com::ispirata::Hemera::DBusObject *applicationManagerObjectInterface;

    ApplicationUpdates applicationUpdates;
    ApplicationPackages installedApplications;
    QDateTime lastCheckForUpdates;
};

void ApplicationManagerPrivate::initHook()
{
    Hemera::AsyncInitObjectPrivate::initHook();
}

bool ApplicationManagerPrivate::checkApplicationUpdatesCoherency(const ApplicationUpdates &updates)
{
    // We need to check whether all of those updates are actually present in our available updates.
    for (const ApplicationUpdate &update : updates) {
        if (!applicationUpdates.contains(update)) {
            qCWarning(LOG_HEMERA_APPLICATIONMANAGER) << ApplicationManager::tr("Requested incoherent update: %1, %2")
                                                        .arg(update.applicationId(), update.version());
            return false;
        }
    }

    return true;
}

ApplicationManager::ApplicationManager(QObject* parent)
    : AsyncInitObject(*new ApplicationManagerPrivate(this), parent)
{
}

ApplicationManager::~ApplicationManager()
{
}

void ApplicationManager::initImpl()
{
    // First of all, we want to check whether our service is up.
    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(Literals::literal(Literals::DBus::applicationManagerService()))) {
        // Houston, we have a problem
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("There's no Application Manager on this Hemera system, or the Application manager failed on startup. "
                        "It's likely this Hemera appliance has been configured without Application Management support."));
        return;
    }

    Q_D(ApplicationManager);

    d->applicationManagerInterface = new com::ispirata::Hemera::SoftwareManagement::ApplicationManager(
                                                    Literals::literal(Literals::DBus::applicationManagerService()),
                                                    Literals::literal(Literals::DBus::applicationManagerPath()),
                                                    QDBusConnection::systemBus(), this);

    if (!d->applicationManagerInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus interface is not available. The software manager has probably encountered a failure."));
        return;
    }

    d->applicationManagerObjectInterface = new com::ispirata::Hemera::DBusObject(Literals::literal(Literals::DBus::applicationManagerService()),
                                                                                 Literals::literal(Literals::DBus::applicationManagerPath()),
                                                                                 QDBusConnection::systemBus(), this);

    if (!d->applicationManagerObjectInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus object interface is not available. The software manager has probably encountered a failure."));
        return;
    }

    setParts(2);

    // Cache properties and connect to signals
    DBusVariantMapOperation *operation = new DBusVariantMapOperation(d->applicationManagerObjectInterface->allProperties(), this);
    connect(operation, &Operation::finished, [this, d, operation] {
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

        QByteArray applicationUpdatesAsJson = result.value(QStringLiteral("applicationUpdates")).toByteArray();
        QByteArray installedApplicationsAsJson = result.value(QStringLiteral("installedApplications")).toByteArray();
        d->lastCheckForUpdates = QDateTime::fromMSecsSinceEpoch(result.value(QStringLiteral("lastCheckForUpdates")).toLongLong());

        // Convert json
        if (!applicationUpdatesAsJson.isEmpty()) {
            d->applicationUpdates = Constructors::applicationUpdatesFromJson(QJsonDocument::fromJson(applicationUpdatesAsJson).array());
        }
        if (!installedApplicationsAsJson.isEmpty()) {
            d->installedApplications = Constructors::applicationPackagesFromJson(QJsonDocument::fromJson(installedApplicationsAsJson).array());
        }

        setOnePartIsReady();
    });

    connect(d->applicationManagerObjectInterface, &com::ispirata::Hemera::DBusObject::propertiesChanged, [this, d] (const QVariantMap &changed) {
        if (changed.contains(QStringLiteral("applicationUpdates"))) {
            d->applicationUpdates = Constructors::applicationUpdatesFromJson(QJsonDocument::fromJson(changed.value(QStringLiteral("applicationUpdates")).toByteArray()).array());
            Q_EMIT availableApplicationUpdatesChanged();
        }

        if (changed.contains(QStringLiteral("lastCheckForUpdates"))) {
            d->lastCheckForUpdates = QDateTime::fromMSecsSinceEpoch(changed.value(QStringLiteral("lastCheckForUpdates")).toLongLong());
            Q_EMIT lastCheckForApplicationUpdatesChanged(d->lastCheckForUpdates);
        }
        if (changed.contains(QStringLiteral("installedApplications"))) {
            d->installedApplications = Constructors::applicationPackagesFromJson(QJsonDocument::fromJson(changed.value(QStringLiteral("installedApplications")).toByteArray()).array());
            Q_EMIT installedApplicationsChanged();
        }
    });

    setOnePartIsReady();
}

ApplicationUpdates ApplicationManager::availableApplicationUpdates() const
{
    Q_D(const ApplicationManager);
    return d->applicationUpdates;
}

QDateTime ApplicationManager::lastCheckForApplicationUpdates() const
{
    Q_D(const ApplicationManager);

    return d->lastCheckForUpdates;
}

ApplicationPackages ApplicationManager::installedApplications() const
{
    Q_D(const ApplicationManager);

    return d->installedApplications;
}

Operation *ApplicationManager::checkForApplicationUpdates()
{
    Q_D(ApplicationManager);

    return new DBusVoidOperation(d->applicationManagerInterface->checkForApplicationUpdates(), this);
}

Operation* ApplicationManager::downloadApplicationUpdates(const ApplicationUpdates& updates)
{
    Q_D(ApplicationManager);

    if (!d->checkApplicationUpdatesCoherency(updates)) {
        return nullptr;
    }

    return new DBusVoidOperation(d->applicationManagerInterface->downloadApplicationUpdates(QJsonDocument(Constructors::toJson(updates)).toJson(QJsonDocument::Compact)), this);
}

Operation* ApplicationManager::updateApplications(const ApplicationUpdates& updates)
{
    Q_D(ApplicationManager);

    if (!d->checkApplicationUpdatesCoherency(updates)) {
        return nullptr;
    }

    return new DBusVoidOperation(d->applicationManagerInterface->updateApplications(QJsonDocument(Constructors::toJson(updates)).toJson(QJsonDocument::Compact)), this);
}

Operation* ApplicationManager::installApplications(const ApplicationPackages &applications)
{
    Q_D(ApplicationManager);

    return new DBusVoidOperation(d->applicationManagerInterface->installApplications(QJsonDocument(Constructors::toJson(applications)).toJson(QJsonDocument::Compact)), this);
}

Operation* ApplicationManager::removeApplications(const ApplicationPackages &applications)
{
    Q_D(ApplicationManager);

    return new DBusVoidOperation(d->applicationManagerInterface->removeApplications(QJsonDocument(Constructors::toJson(applications)).toJson(QJsonDocument::Compact)), this);
}

}

}
