/*
 *
 */

#ifndef HEMERA_QMLAPPLIANCEMANAGER_H
#define HEMERA_QMLAPPLIANCEMANAGER_H

#include <HemeraSoftwareManagement/ApplianceManager>

#include <QtCore/QDateTime>

namespace Hemera {

namespace Qml {

namespace SoftwareManagement {

class ApplianceManagerPrivate;
class SystemUpdate;
/**
 * @class ApplianceManager
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagementappliancemanager.h <HemeraSoftwareManagement/ApplianceManager>
 *
 * @brief Provides access to Hemera's software management, such as updates and application installation and removal.
 *
 * ApplianceManager enables an application to act as a software manager, by updating the system and its applications,
 * and installing and removing applications.
 *
 * Usually, only one single ApplianceManager object should be instantiated for each application for performance reasons.
 *
 * @par Security and permissions
 *
 * By default, every Hemera application has read-only access to the ApplianceManager. Access to operations can be enabled
 * by enabling more features in the application's manifest file. In case a method is called and the application has
 * no permissions to execute it, the corresponding Operation will return @ref Hemera::Literals::Error::notAllowed.
 *
 * @par System-wide availability of ApplianceManager
 *
 * ApplianceManager is part of Gravity, as explained in @ref SoftwareManagementInHemera "Software Management in Hemera", and
 * can be entirely disabled from the vendor. This class serves as a mere interface to Gravity, and its initialization will
 * fail if the system ApplianceManager is not installed or not available.
 *
 * @sa Application
 */
class HEMERA_QT5_SDK_EXPORT ApplianceManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ApplianceManager)

    /// The last occurrence of an update check
    Q_PROPERTY(QDateTime            lastCheckForUpdates          READ lastCheckForUpdates         NOTIFY lastCheckForUpdatesChanged)
    Q_PROPERTY(bool                 checkingForUpdates           READ isCheckingForUpdates        NOTIFY checkingForUpdatesChanged)
    /// Available system update, if any
    Q_PROPERTY(Hemera::Qml::SoftwareManagement::SystemUpdate* availableSystemUpdate READ availableSystemUpdate NOTIFY availableSystemUpdateChanged)
    Q_PROPERTY(QString              applianceName                READ applianceName)
    Q_PROPERTY(QString              applianceVariant             READ applianceVariant)
    Q_PROPERTY(QString              applianceVersion             READ applianceVersion            NOTIFY applianceVersionChanged)
    Q_PROPERTY(QString              hemeraVersion                READ hemeraVersion               NOTIFY hemeraVersionChanged)
    Q_PROPERTY(QString              errorName                    READ errorName)
    Q_PROPERTY(QString              errorMessage                 READ errorMessage)

public:
    explicit ApplianceManager(QObject *parent = nullptr);
    virtual ~ApplianceManager();

    /**
     * @brief Returns the last occurrence of a check for updates.
     *
     * @returns The last occurrence of a check for updates as a QDateTime object.
     */
    QDateTime lastCheckForUpdates() const;

    bool isCheckingForUpdates() const;

    QString applianceName() const;
    QString applianceVariant() const;
    QString applianceVersion() const;
    QString hemeraVersion() const;

    QString errorName() const;
    QString errorMessage() const;
    /**
     * @brief Returns the available system update, if any.
     *
     * This update refers to the last update set retrieved on @ref lastCheckForUpdates.
     *
     * @returns The available system update, or an invalid SystemUpdate object if no update is available.
     *
     * @sa lastCheckForUpdates
     * @sa SystemUpdate
     */
    SystemUpdate *availableSystemUpdate() const;

    /**
     * @brief Checks for available application and system updates.
     *
     * This operation forces a refresh of all sources known to the system at the time of this call.
     * This includes network updates, removable media updates, USB OTG updates and any media supported by the system.
     * Once the operation is completed, availableApplicationUpdates and availableSystemUpdate might be updated
     * with new values.
     *
     * @note This method always returns a valid Operation
     * @note This method requires feature @e CheckForUpdates to be enabled for being executed successfully.
     *
     * @returns An @ref Operation representing the update check.
     */
    Q_INVOKABLE void checkForUpdates(Hemera::SoftwareManagement::SystemUpdate::UpdateType preferredUpdateType =
                                        Hemera::SoftwareManagement::SystemUpdate::UpdateType::IncrementalUpdate);

    /**
     * @brief Downloads the available system update.
     *
     * This operation downloads and caches to the system the available system update, making it ready for installation.
     *
     * @note There must be an available system update to call this method. If no system update is available, the
     *       method will return a null pointer.
     * @note A system update might require a lot of available disk space, be sure to check the disk before starting
     *       the operation. Also, @ref SystemUpdate always reports its download and installation size.
     * @note The system might be configured to automatically download updates.
     * @note This method requires feature @e DownloadUpdates to be enabled for being executed successfully.
     *
     * @returns An @ref Operation representing the update download, or a null pointer if no system update is available.
     */
    Q_INVOKABLE void downloadSystemUpdate();

    /**
     * @brief Updates the system
     *
     * This operation updates the system to a new release. The operation will first download all the update, and
     * install it afterwards. You don't need to call @ref downloadSystemUpdate before this method. If
     * @ref downloadSystemUpdate was called previously or the system is configured for automatically downloading
     * updates, the download phase will likely be skipped.
     *
     * @par Update procedure
     *
     * A system update is always performed in a protected environment to minimize any potential risk of compromising
     * the system. As such, the update will be first downloaded, the system will then reboot in a special <em>update mode</em>.
     * It is possible to supply Hemera with a QML interface to display progress during the system update.
     *
     * In Hemera's <em>update mode</em>, pretty much every system feature is disabled - Gravity itself will boot in <em>update mode</em>
     * and will disable orbit management. As such, provided QML files can make no use of Hemera's SDK, except for the @e resource protocol.
     *
     * @note There must be an available system update to call this method. If no system update is available, the
     *       method will return a null pointer.
     * @note A system update might require a lot of available disk space, be sure to check the disk before starting
     *       the operation. Also, @ref SystemUpdate always reports its download and installation size.
     * @note This method requires feature @e UpdateSystem to be enabled for being executed successfully.
     * @note Due to the behavior of the system update, the operation will complete once the system starts rebooting in <em>update mode</em>.
     *
     * @returns An @ref Operation representing the update procedure, or a null pointer if no system update is available.
     */
    Q_INVOKABLE void updateSystem();

Q_SIGNALS:
    /**
     * @brief Emitted when available system and/or application updates changed.
     */
    void availableSystemUpdateChanged();
    /**
     * @brief Emitted when the system is successfully checked for updates, and the last check occurrence changes.
     */
    void lastCheckForUpdatesChanged(const QDateTime &lastCheck);

    void checkingForUpdatesChanged();

    void applianceVersionChanged();
    void hemeraVersionChanged();

    void downloadSystemUpdateFinished();
    void updateSystemFinished();
    void checkForUpdatesFinished();

    void error();

private:
    class Private;
    Private * const d;

};
}

}

}

#endif // HEMERA_QMLAPPLIANCEMANAGER_H
