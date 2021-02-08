#ifndef HEMERA_SERVICEMANAGER_H
#define HEMERA_SERVICEMANAGER_H

#include <HemeraCore/AsyncInitObject>

#include <QtCore/QSharedDataPointer>

namespace Hemera {

class ServiceInfoData;
class ServiceInfo {
public:
    /// Creates an invalid Service Info
    ServiceInfo();
    /// Copy constructor
    ServiceInfo(const ServiceInfo &other);
    ~ServiceInfo();
    enum class ServiceType {
        Unknown = HEMERA_ENUM_ServiceInfo_ServiceType_Unknown,
        Application = HEMERA_ENUM_ServiceInfo_ServiceType_Application,
        Orbit = HEMERA_ENUM_ServiceInfo_ServiceType_Orbit,
        RootOperation = HEMERA_ENUM_ServiceInfo_ServiceType_RootOperation
    };

    ServiceInfo &operator=(const ServiceInfo &rhs);
    bool operator==(const ServiceInfo &other) const;
    inline bool operator!=(const ServiceInfo &other) const { return !operator==(other); }

    /**
     * @returns Whether this service info is valid or not.
     */
    bool isValid() const;

    ServiceType type() const;

    QString id() const;
    QString name() const;
    QString version() const;
    QString description() const;
    QUrl icon() const;

    QStringList handledMimeTypes() const;

    QVariantMap serviceData() const;

private:
    QSharedDataPointer<ServiceInfoData> d;

    explicit ServiceInfo(ServiceInfoData *d);

    friend class ServiceManager;
    friend class ServiceManagerPrivate;
};
typedef QList<ServiceInfo> ServiceInfoList;


class ServiceManagerPrivate;
/**
 * @class ServiceManager
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemeraservicemanager.h <HemeraCore/ServiceManager>
 *
 * @brief Provides access to this system's installed services, such as Applications and more.
 *
 * ServiceManager enables an application to introspect the system for available services, and access their metadata.
 * ServiceManager performs the discovery on the actual system, and not in the
 * software repositories or the software database.
 *
 * Usually, only one single ServiceManager object should be instantiated for each application for performance reasons.
 *
 * @sa Hemera::Application
 */
class ServiceManager : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServiceManager)
    Q_DECLARE_PRIVATE_D(d_h_ptr, ServiceManager)

    Q_PRIVATE_SLOT(d_func(), void rescanServices())

public:
    explicit ServiceManager(QObject *parent = nullptr);
    virtual ~ServiceManager();

    /**
     *
     */
    ServiceInfoList availableServices() const;
    /**
     * Finds an installed service on the system.
     *
     * @p applicationId The ID of the service to retrieve
     *
     * @returns A valid ServiceInfo object if the service is found, an invalid ServiceInfo if not
     */
    ServiceInfo findServiceById(const QString &serviceId) const;
    /**
     * Finds installed services on the system capable of handling a mimetype.
     *
     * @p mimeType The mimetype to look up.
     *
     * @returns A list of valid ServiceInfo objects capable of handling the chosen mimetype.
     */
    ServiceInfoList findServicesHandlingMimeType(const QString &mimeType) const;

    /**
     * Retrieves all the Orbital Applications in the system.
     *
     * It is the equivalent of calling services(Application), with the difference that it is
     * guaranteed that calling @ref orbitForApplication on any of the returned services will
     * return a meaningful value which represents an Orbital application.
     *
     * This is likely the most useful mechanism for building application launchers.
     */
    ServiceInfoList orbitalApplications() const;

    /**
     * @returns Available services of the chosen type.
     */
    ServiceInfoList services(ServiceInfo::ServiceType type) const;

    /**
     * Retrieves the best orbit for the chosen application ID.
     *
     * This will return either the Orbital Application service, if it exists, or the first
     * Orbit which features the chosen application.
     *
     * @return A ServiceInfo representing an Orbit containing @p applicationId, or an invalid ServiceInfo.
     */
    ServiceInfo orbitForApplication(const QString &applicationId);
    /**
     * Convenience overload, useful in conjunction with @ref orbitalApplications
     */
    ServiceInfo orbitForApplication(const ServiceInfo &application);

    /**
     * @returns A list of the application IDs the Service provides.
     */
    QStringList applicationsForService(const ServiceInfo &service);

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

Q_SIGNALS:
    /**
     * This signal is emitted when services on the system are either installed or removed.
     */
    void servicesChanged();
};
}

Q_DECLARE_METATYPE(Hemera::ServiceInfo)

#endif // HEMERA_SERVICEMANAGER_H
