#ifndef HEMERACXX_SERVICEMANAGER_H
#define HEMERACXX_SERVICEMANAGER_H

#include <HemeraCore/Global>

#include <QtCore/QSharedDataPointer>

namespace Hemera {
class ServiceInfo;
}

namespace HemeraCxx {

class ServiceInfoData;
class ServiceInfo {
public:
    /// Creates an invalid Service Info
    ServiceInfo();
    /// Copy constructor
    ServiceInfo(const ServiceInfo &other);
    ServiceInfo(const Hemera::ServiceInfo &other);
    ~ServiceInfo();
    enum class ServiceType {
        Unknown = HEMERA_ENUM_ServiceInfo_ServiceType_Unknown,
        Application = HEMERA_ENUM_ServiceInfo_ServiceType_Application,
        Orbit = HEMERA_ENUM_ServiceInfo_ServiceType_Orbit
    };

    ServiceInfo &operator=(const ServiceInfo &rhs);
    bool operator==(const ServiceInfo &other) const;
    inline bool operator!=(const ServiceInfo &other) const { return !operator==(other); }

    /**
     * @returns Whether this service info is valid or not.
     */
    bool isValid() const;

    ServiceType type() const;

    std::string id() const;
    std::string name() const;
    std::string version() const;
    std::string description() const;
    std::string icon() const;

    std::vector< std::string > handledMimeTypes() const;

    // This is a JSON payload
    std::string serviceData() const;

private:
    QSharedDataPointer<ServiceInfoData> d;

    explicit ServiceInfo(ServiceInfoData *d);

    friend class ServiceManager;
    friend class ServiceManagerPrivate;
    friend class ServiceInfoData;
};
typedef std::vector< HemeraCxx::ServiceInfo > ServiceInfoList;

// Wrappers
typedef ServiceInfo ServiceInfoWrapper;
typedef ServiceInfoList ServiceInfoListWrapper;

class ServiceManager
{
public:
    ServiceManager();
    virtual ~ServiceManager();

    typedef void (*_services_changed_cb)(void*);

    static ServiceManager *serviceManagerInstance();

    ServiceInfoList availableServices() const;
    ServiceInfo findServiceById(const std::string &serviceId) const;
    ServiceInfoList findServicesHandlingMimeType(const std::string &mimeType) const;
    ServiceInfoList orbitalApplications() const;
    ServiceInfoList services(ServiceInfo::ServiceType type) const;
    ServiceInfo orbitForApplication(const std::string &applicationId);
    ServiceInfo orbitForApplication(const ServiceInfo &application);
    std::vector< std::string > applicationsForService(const ServiceInfo &service);

    int registerServicesChangedCallback(_services_changed_cb cb, void *data = nullptr);
    void unregisterServicesChangedCallback(int cbId);

private:
    class Private;
    Private * const d;
};

// Wrappers
typedef ServiceManager ServiceManagerWrapper;

}


#endif // HEMERACXX_SERVICEMANAGER_H
