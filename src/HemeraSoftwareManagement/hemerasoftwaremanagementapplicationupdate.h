#ifndef HEMERA_APPLICATIONUPDATE_H
#define HEMERA_APPLICATIONUPDATE_H

#include <HemeraCore/Global>

#include <QtCore/QList>
#include <QtCore/QJsonObject>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

namespace Hemera {

namespace SoftwareManagement {

class ApplicationUpdateData;
/**
 * @class ApplicationUpdate
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagementapplicationupdate.h <HemeraSoftwareManagement/ApplicationUpdate>
 *
 * @brief Represents an available, and uninstalled, application update
 *
 * ApplicationUpdate represents an update known to the SoftwareManager. This class is meant
 * to review the update and to submit the update to the SoftwareManager in case it should
 * be installed.
 *
 * @note ApplicationUpdate is an implicitly shared object.
 *
 * @sa Hemera::SoftwareManagement::SoftwareManager
 */
class HEMERA_QT5_SDK_EXPORT ApplicationUpdate {
public:
    ApplicationUpdate();
    ApplicationUpdate(const ApplicationUpdate &other);
    ~ApplicationUpdate();

    ApplicationUpdate &operator=(const ApplicationUpdate &rhs);
    bool operator==(const ApplicationUpdate &other) const;
    inline bool operator!=(const ApplicationUpdate &other) const { return !operator==(other); }

    /// @returns Whether this application update is valid or not
    bool isValid() const;

    /// @returns The application ID of the updateable application
    QString applicationId() const;
    /// @returns The application name of the updateable application
    QString applicationName() const;
    /// @returns The application description of the updateable application
    QString description() const;
    /// @returns The installed version of the updateable application
    QString installedVersion() const;
    /// @returns The version the application can be updated to
    QString version() const;
    /// @returns The download size of the update
    quint64 downloadSize() const;
    /// @returns The delta, in bytes, of the installed application size after the update
    qint32 installedSize() const;
    /// @returns The changelog between updates of the updateable application
    QString changelog() const;

private:
    QSharedDataPointer<ApplicationUpdateData> d;

    explicit ApplicationUpdate(ApplicationUpdateData *data);

    friend class Constructors;
};

/**
 * @ingroup HemeraSoftwareManagement
 *
 * Convenience typedef to represent a list of @ref Hemera::SoftwareManagement::ApplicationUpdate
 */
typedef QList< Hemera::SoftwareManagement::ApplicationUpdate > ApplicationUpdates;

}

}

#endif // HEMERA_APPLICATIONUPDATE_H
