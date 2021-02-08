/*
 *
 */

#ifndef HEMERA_APPLICATIONPACKAGE_H
#define HEMERA_APPLICATIONPACKAGE_H

#include <HemeraCore/Global>

#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

class QUrl;
namespace Hemera {

namespace SoftwareManagement {

class ApplicationPackageData;
/**
 * @class ApplicationPackage
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagementapplicationpackage.h <HemeraSoftwareManagement/ApplicationPackage>
 *
 * @brief Represents an application on the system, or an installable application
 *
 * ApplicationPackage represents an application on the system. The application could be installed and
 * known to the software manager, installed and recognized as a service (so information are limited),
 * uninstalled and known to the package manager, or generated from a package file (e.g.: the result of
 * _hsdk full-build_). Depending on the case, it might or might not have some fields available.
 *
 * @note ApplicationPackage is an implicitly shared object.
 *
 * @sa Hemera::SoftwareManagement::SoftwareManager
 * @sa Hemera::SoftwareManagement::ServiceManager
 */
class HEMERA_QT5_SDK_EXPORT ApplicationPackage {
public:
    /// Creates an invalid Application package
    ApplicationPackage();
    /// Copy constructor
    ApplicationPackage(const ApplicationPackage &other);
    ~ApplicationPackage();

    ApplicationPackage &operator=(const ApplicationPackage &rhs);
    bool operator==(const ApplicationPackage &other) const;
    inline bool operator!=(const ApplicationPackage &other) const { return !operator==(other); }

    /**
     * @returns Whether this application package is valid or not.
     */
    bool isValid() const;

    /// @returns The application ID of the application
    QString applicationId() const;
    /// @returns The application name of the application
    QString applicationName() const;
    /// @returns The description of the application
    QString description() const;
    /**
     * @returns The URL to the icon of the application
     *
     * @note This field might be empty depending on the source of the ApplicationPackage
     */
    QUrl icon() const;
    /**
     * @returns The package name of the application
     *
     * @note This field might be empty depending on the source of the ApplicationPackage
     */
    QString packageName() const;
    /// @returns The version of the application
    QString version() const;
    /**
     * @returns The size, in bytes, of the package of the application
     *
     * @note This field might be empty depending on the source of the ApplicationPackage
     */
    quint64 packageSize() const;
    /**
     * @returns The installed size, in bytes, of the application
     *
     * @note This field might be empty depending on the source of the ApplicationPackage
     */
    quint64 installedSize() const;

    /// @returns Whether the application is installed or not
    bool isInstalled() const;

private:
    QSharedDataPointer<ApplicationPackageData> d;

    explicit ApplicationPackage(ApplicationPackageData *data);

    friend class Constructors;
    // For injecting the icon
    friend class ServiceManagerPrivate;
};

/**
 * @ingroup HemeraSoftwareManagement
 *
 * Convenience typedef to represent a list of @ref Hemera::SoftwareManagement::ApplicationPackage
 */
typedef QList< Hemera::SoftwareManagement::ApplicationPackage > ApplicationPackages;

}

}

#endif // HEMERA_APPLICATIONPACKAGE_H
