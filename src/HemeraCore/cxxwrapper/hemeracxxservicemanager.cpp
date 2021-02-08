#include "hemeracxxservicemanager.h"

#include <HemeraCxxCore/Global>

#include <HemeraCore/Operation>
#include <HemeraCore/ServiceManager>

#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace HemeraCxx {

class ServiceInfoData : public QSharedData
{
public:
    ServiceInfoData() { }
    ServiceInfoData(const ServiceInfoData &other)
        : QSharedData(other), proxy(other.proxy) { }
    ServiceInfoData(const Hemera::ServiceInfo &data)
        : proxy(data) {}
    ~ServiceInfoData() { }

    Hemera::ServiceInfo proxy;

    static Hemera::ServiceInfoList fromCxx(const ServiceInfoList &updates);
    static ServiceInfoList toCxx(const Hemera::ServiceInfoList &updates);
};

Hemera::ServiceInfoList ServiceInfoData::fromCxx(const HemeraCxx::ServiceInfoList &packages)
{
    Hemera::ServiceInfoList ret;
    // Convert
    for (const HemeraCxx::ServiceInfo &package : packages) {
        ret.push_back(package.d->proxy);
    }
    return ret;
}

HemeraCxx::ServiceInfoList ServiceInfoData::toCxx(const Hemera::ServiceInfoList &packages)
{
    HemeraCxx::ServiceInfoList ret;
    // Convert
    for (const Hemera::ServiceInfo &package : packages) {
        ret.push_back(HemeraCxx::ServiceInfo(package));
    }
    return ret;
}

ServiceInfo::ServiceInfo()
    : d(new ServiceInfoData())
{
}

ServiceInfo::ServiceInfo(ServiceInfoData* data)
    : d(data)
{
}

ServiceInfo::ServiceInfo(const Hemera::ServiceInfo &other)
    : d(new ServiceInfoData(other))
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
    return d->proxy == other.d->proxy;
}

bool ServiceInfo::isValid() const
{
    return !d->proxy.isValid();
}

std::string ServiceInfo::description() const
{
    return d->proxy.description().toStdString();
}

std::vector< std::string > ServiceInfo::handledMimeTypes() const
{
    return qStringListToVector(d->proxy.handledMimeTypes());
}

std::string ServiceInfo::icon() const
{
    return d->proxy.icon().toString().toStdString();
}

std::string ServiceInfo::id() const
{
    return d->proxy.id().toStdString();
}

std::string ServiceInfo::name() const
{
    return d->proxy.name().toStdString();
}

std::string ServiceInfo::serviceData() const
{
    return QString::fromLatin1(QJsonDocument(QJsonObject::fromVariantMap(d->proxy.serviceData())).toJson(QJsonDocument::Compact)).toStdString();
}

ServiceInfo::ServiceType ServiceInfo::type() const
{
    return static_cast<ServiceInfo::ServiceType>(d->proxy.type());
}

std::string ServiceInfo::version() const
{
    return d->proxy.version().toStdString();
}



class ServiceManager::Private
{
public:
    Private() : servicesChangedCbId(0) {}

    Hemera::ServiceManager *proxy;

    QHash< int, QPair< _services_changed_cb, void* > > servicesChangedCbs;
    int servicesChangedCbId;

    // Proxied slots
    void onServicesChanged();
};

void ServiceManager::Private::onServicesChanged()
{
    // Invoke every registered callback
    for (const QPair< _services_changed_cb, void* > &cb : servicesChangedCbs) {
        cb.first(cb.second);
    }
}

class ServiceManagerHelper
{
public:
    ServiceManagerHelper() : q(nullptr) {}
    ~ServiceManagerHelper() {
        // Do not delete - the object will be cleaned up already.
    }
    ServiceManager *q;
};

Q_GLOBAL_STATIC(ServiceManagerHelper, s_globalCxxServiceManager)

ServiceManager *ServiceManager::serviceManagerInstance()
{
    if (!s_globalCxxServiceManager()->q) {
        // Create a new instance
        new ServiceManager;
    }

    return s_globalCxxServiceManager()->q;
}

ServiceManager::ServiceManager()
    : d(new Private)
{
    Q_ASSERT(!s_globalCxxServiceManager()->q);
    s_globalCxxServiceManager()->q = this;

    d->proxy = new Hemera::ServiceManager();

    // Proxy signals/slots thanks to amazing C++11 power.
    QObject::connect(d->proxy, &Hemera::ServiceManager::servicesChanged, [this] { d->onServicesChanged(); });

    d->proxy->init()->synchronize();
}

ServiceManager::~ServiceManager()
{
    s_globalCxxServiceManager()->q = nullptr;
    d->proxy->deleteLater();
    delete d;
}

std::vector< std::string > ServiceManager::applicationsForService(const ServiceInfo& service)
{
    return qStringListToVector(d->proxy->applicationsForService(service.d->proxy));
}

ServiceInfoList ServiceManager::availableServices() const
{
    return ServiceInfoData::toCxx(d->proxy->availableServices());
}

ServiceInfo ServiceManager::findServiceById(const std::string& serviceId) const
{
    return ServiceInfo(d->proxy->findServiceById(QString::fromStdString(serviceId)));
}

ServiceInfoList ServiceManager::findServicesHandlingMimeType(const std::string& mimeType) const
{
    return ServiceInfoData::toCxx(d->proxy->findServicesHandlingMimeType(QString::fromStdString(mimeType)));
}

ServiceInfoList ServiceManager::orbitalApplications() const
{
    return ServiceInfoData::toCxx(d->proxy->orbitalApplications());
}

ServiceInfo ServiceManager::orbitForApplication(const ServiceInfo& application)
{
    return ServiceInfo(d->proxy->orbitForApplication(application.d->proxy));
}

ServiceInfo ServiceManager::orbitForApplication(const std::string& applicationId)
{
    return ServiceInfo(d->proxy->orbitForApplication(QString::fromStdString(applicationId)));
}

ServiceInfoList ServiceManager::services(ServiceInfo::ServiceType type) const
{
    return ServiceInfoData::toCxx(d->proxy->services(static_cast<Hemera::ServiceInfo::ServiceType>(type)));
}

int ServiceManager::registerServicesChangedCallback(ServiceManager::_services_changed_cb cb, void* data)
{
    ++d->servicesChangedCbId;
    d->servicesChangedCbs.insert(d->servicesChangedCbId, qMakePair(cb, data));
    return d->servicesChangedCbId;
}

void ServiceManager::unregisterServicesChangedCallback(int cbId)
{
    d->servicesChangedCbs.remove(cbId);
}

}
