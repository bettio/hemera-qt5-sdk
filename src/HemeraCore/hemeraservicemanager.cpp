#include "hemeraservicemanager.h"

#include "hemeraasyncinitobject_p.h"

#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QJsonDocument>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUrl>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <hemeractconfig.h>

Q_LOGGING_CATEGORY(LOG_HEMERA_SERVICEMANAGER, "Hemera::ServiceManager")

namespace Hemera {

class ServiceInfoData : public QSharedData
{
public:
    ServiceInfoData() : type(ServiceInfo::ServiceType::Unknown) { }
    ServiceInfoData(ServiceInfo::ServiceType type, const QString &id, const QString &name, const QString &version, const QString &description, const QUrl &icon,
                    const QStringList &handledMimeTypes, const QVariantMap &serviceData)
        : type(type), id(id), name(name), version(version), description(description), icon(icon), handledMimeTypes(handledMimeTypes), serviceData(serviceData) { }
    ServiceInfoData(const ServiceInfoData &other)
        : QSharedData(other), type(other.type), id(other.id), name(other.name), version(other.version), description(other.description), icon(other.icon)
        , handledMimeTypes(other.handledMimeTypes), serviceData(other.serviceData) { }
    ~ServiceInfoData() { }

    ServiceInfo::ServiceType type;

    QString id;
    QString name;
    QString version;
    QString description;
    QUrl icon;

    QStringList handledMimeTypes;

    QVariantMap serviceData;
};

ServiceInfo::ServiceInfo()
    : d(new ServiceInfoData())
{
}

ServiceInfo::ServiceInfo(ServiceInfoData* data)
    : d(data)
{
}

ServiceInfo::ServiceInfo(const ServiceInfo& other)
    : d(other.d)
{
}

ServiceInfo::~ServiceInfo()
{

}

ServiceInfo& ServiceInfo::operator=(const ServiceInfo& rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool ServiceInfo::operator==(const ServiceInfo& other) const
{
    return d->id == other.id() && d->version == other.version();
}

bool ServiceInfo::isValid() const
{
    return !d->id.isEmpty() && d->type != ServiceType::Unknown;
}

ServiceInfo::ServiceType ServiceInfo::type() const
{
    return d->type;
}

QString ServiceInfo::id() const
{
    return d->id;
}

QString ServiceInfo::name() const
{
    return d->name;
}

QString ServiceInfo::description() const
{
    return d->description;
}

QUrl ServiceInfo::icon() const
{
    return d->icon;
}

QString ServiceInfo::version() const
{
    return d->version;
}

QStringList ServiceInfo::handledMimeTypes() const
{
    return d->handledMimeTypes;
}

QVariantMap ServiceInfo::serviceData() const
{
    return d->serviceData;
}


////////////////////////////////

class ServiceManagerPrivate : public AsyncInitObjectPrivate
{
public:
    ServiceManagerPrivate(ServiceManager *q) : AsyncInitObjectPrivate(q) {}
    virtual ~ServiceManagerPrivate() {}

    Q_DECLARE_PUBLIC(ServiceManager)

    virtual void initHook() Q_DECL_OVERRIDE;

    void rescanServices();

    QFileSystemWatcher *fsWatcher;

    QHash< QString, ServiceInfo > services;
    QMultiHash< QString, ServiceInfo > servicesForMimeTypes;
    QMultiHash< uint, ServiceInfo > servicesByType;
    QHash< QString, ServiceInfo > orbitalApplications;
    QHash< QString, QString > orbitsForOrbitalApplications;
};

void ServiceManagerPrivate::initHook()
{
    Hemera::AsyncInitObjectPrivate::initHook();
}

void ServiceManagerPrivate::rescanServices()
{
    // Clear
    services.clear();
    servicesForMimeTypes.clear();
    servicesByType.clear();
    orbitalApplications.clear();
    orbitsForOrbitalApplications.clear();

    // Let's go through the dirs
    QDir dir(QLatin1String(Hemera::StaticConfig::haFilesPath()));
    QDir iconDir(QLatin1String(Hemera::StaticConfig::iconsDir()));
    dir.setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files);
    iconDir.setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files);
    dir.setNameFilters(QStringList() << QStringLiteral("*.hemeraservice"));

    ServiceInfoList candidateOrbitalApplications;

    for (const QString &service : dir.entryList()) {
        QFile file(QStringLiteral("%1%2%3").arg(QLatin1String(Hemera::StaticConfig::haFilesPath()), QDir::separator(), service));
        if (!file.open(QFile::ReadOnly)) {
            qCWarning(LOG_HEMERA_SERVICEMANAGER) << ServiceManager::tr("Could not open service file %1 for reading!").arg(service);
            continue;
        }

        QJsonParseError error;
        QJsonDocument data = QJsonDocument::fromJson(file.readAll(), &error);

        if (error.error != QJsonParseError::NoError || data.isEmpty() || !data.isObject()) {
            qCWarning(LOG_HEMERA_SERVICEMANAGER) << ServiceManager::tr("Error while parsing service file %1!").arg(service);
            continue;
        }

        auto variantListToStringList = [] (const QVariantList &list) -> QStringList { QStringList ret; for (const QVariant &v:list) ret.append(v.toString()); return ret; };
        // What kind of service is this?
        QJsonObject serviceObject = data.object();

        ServiceInfo::ServiceType type = static_cast<ServiceInfo::ServiceType>(serviceObject.value(QStringLiteral("type")).toInt());
        QString serviceId = serviceObject.value(QStringLiteral("id")).toString();
        QStringList handledMimeTypes = variantListToStringList(serviceObject.value(QStringLiteral("handledMimeTypes")).toArray().toVariantList());

        // Find icon
        QUrl iconPath;
        QFileInfoList icons = iconDir.entryInfoList(QStringList() << QStringLiteral("%1.*").arg(serviceId));
        if (icons.size() > 1) {
            // Whoa?
            qCWarning(LOG_HEMERA_SERVICEMANAGER)
                << ServiceManager::tr("Found more than a matching icon for %1! Something bad is going on.").arg(serviceId);
        } else if (icons.size() == 1) {
            iconPath = QUrl::fromLocalFile(icons.first().absoluteFilePath());
        }

        QVariantMap serviceData = serviceObject.value(QStringLiteral("serviceData")).toObject().toVariantMap();

        ServiceInfo serviceInfo(new ServiceInfoData(type, serviceId, serviceObject.value(QStringLiteral("name")).toString(),
                                                    serviceObject.value(QStringLiteral("version")).toString(),
                                                    serviceObject.value(QStringLiteral("description")).toString(), iconPath, handledMimeTypes,
                                                    serviceData));

        // Add.
        services.insert(serviceId, serviceInfo);
        for (const QString &mimetype : handledMimeTypes) {
            servicesForMimeTypes.insert(mimetype, serviceInfo);
        }
        servicesByType.insert(static_cast<uint>(type), serviceInfo);
        if (serviceData.value(QStringLiteral("isOrbitalApplication")).toBool()) {
            candidateOrbitalApplications << serviceInfo;
        }
    }

    // Iterate candidate orbital applications
    for (const Hemera::ServiceInfo &info : candidateOrbitalApplications) {
        for (const QString &application : info.serviceData().value(QStringLiteral("applications")).toStringList()) {
            if (services.contains(application)) {
                orbitalApplications.insert(application, services.value(application));
                orbitsForOrbitalApplications.insert(application, info.id());
            }
        }
    }

    // If we're ready, notify
    Q_Q(ServiceManager);
    if (q->isReady()) {
        Q_EMIT q->servicesChanged();
    }
}

ServiceManager::ServiceManager(QObject* parent)
    : AsyncInitObject(*new ServiceManagerPrivate(this), parent)
{
}


ServiceManager::~ServiceManager()
{
}

void ServiceManager::initImpl()
{
    Q_D(ServiceManager);

    // Monitor filesystem
    d->fsWatcher = new QFileSystemWatcher(this);
    d->fsWatcher->addPath(QLatin1String(Hemera::StaticConfig::haFilesPath()));
    connect(d->fsWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(rescanServices()));

    // Rescan services before we begin.
    d->rescanServices();

    setReady();
}

ServiceInfoList ServiceManager::availableServices() const
{
    Q_D(const ServiceManager);
    return d->services.values();
}

ServiceInfoList ServiceManager::services(ServiceInfo::ServiceType type) const
{
    Q_D(const ServiceManager);
    return d->servicesByType.values(static_cast<uint>(type));
}

ServiceInfoList ServiceManager::orbitalApplications() const
{
    Q_D(const ServiceManager);
    return d->orbitalApplications.values();
}

ServiceInfo ServiceManager::findServiceById(const QString& serviceId) const
{
    Q_D(const ServiceManager);
    return d->services.value(serviceId);
}

ServiceInfoList ServiceManager::findServicesHandlingMimeType(const QString &mimeType) const
{
    Q_D(const ServiceManager);
    return d->servicesForMimeTypes.values(mimeType);
}

ServiceInfo ServiceManager::orbitForApplication(const QString &applicationId)
{
    Q_D(const ServiceManager);
    // Iterate orbits.
    if (d->orbitalApplications.contains(applicationId)) {
        return d->services.value(d->orbitsForOrbitalApplications.value(applicationId));
    }

    for (const ServiceInfo &info : services(ServiceInfo::ServiceType::Orbit)) {
        if (info.serviceData().value(QStringLiteral("applications")).toStringList().contains(applicationId)) {
            return info;
        }
    }

    return ServiceInfo();
}

ServiceInfo ServiceManager::orbitForApplication(const ServiceInfo& application)
{
    if (application.type() != ServiceInfo::ServiceType::Application || !application.isValid()) {
        return ServiceInfo();
    }

    return orbitForApplication(application.id());
}

QStringList ServiceManager::applicationsForService(const ServiceInfo& service)
{
    QStringList ret;
    switch (service.type()) {
        case ServiceInfo::ServiceType::Application:
            ret << service.id();
            break;
        case ServiceInfo::ServiceType::Orbit:
            ret << service.serviceData().value(QStringLiteral("applications")).toStringList();
            break;
        default:
            break;
    }

    return ret;
}

}

#include "moc_hemeraservicemanager.cpp"
