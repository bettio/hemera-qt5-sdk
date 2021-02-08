/*
 *
 */

#ifndef HEMERA_APPLICATIONMANAGER_H
#define HEMERA_APPLICATIONMANAGER_H

#include <HemeraCore/AsyncInitObject>

#include <HemeraSoftwareManagement/ApplicationPackage>
#include <HemeraSoftwareManagement/ApplicationUpdate>

#include <QtCore/QDateTime>

namespace Hemera {

namespace SoftwareManagement {

class ApplicationManagerPrivate;
/**
 * @class ApplicationManager
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagementsoftwaremanager.h <HemeraSoftwareManagement/ApplicationManager>
 *
 * @brief Provides access to Hemera's software management, such as updates and application installation and removal.
 *
 * ApplicationManager enables an application to act as a software manager, by updating the system and its applications,
 * and installing and removing applications.
 *
 * Usually, only one single ApplicationManager object should be instantiated for each application for performance reasons.
 *
 * @par Security and permissions
 *
 * By default, every Hemera application has read-only access to the ApplicationManager. Access to operations can be enabled
 * by enabling more features in the application's manifest file. In case a method is called and the application has
 * no permissions to execute it, the corresponding Operation will return @ref Hemera::Literals::Error::notAllowed.
 *
 * @par System-wide availability of ApplicationManager
 *
 * ApplicationManager is part of Gravity, as explained in @ref SoftwareManagementInHemera "Software Management in Hemera", and
 * can be entirely disabled from the vendor. This class serves as a mere interface to Gravity, and its initialization will
 * fail if the system ApplicationManager is not installed or not available.
 *
 * @sa Application
 */
class HEMERA_QT5_SDK_EXPORT ApplicationManager : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ApplicationManager)
    Q_DECLARE_PRIVATE_D(d_h_ptr, ApplicationManager)

    /// The last occurrence of an update check
    Q_PROPERTY(QDateTime            lastCheckForUpdates          READ lastCheckForApplicationUpdates  NOTIFY lastCheckForApplicationUpdatesChanged)
    Q_PROPERTY(ApplicationUpdates   availableApplicationUpdates  READ availableApplicationUpdates     NOTIFY availableApplicationUpdatesChanged)
    Q_PROPERTY(ApplicationPackages  installedApplications        READ installedApplications           NOTIFY installedApplicationsChanged)

public:
    explicit ApplicationManager(QObject *parent = nullptr);
    virtual ~ApplicationManager();

    /**
     * @brief Returns the last occurrence of a check for updates.
     *
     * @returns The last occurrence of a check for updates as a QDateTime object.
     */
    QDateTime lastCheckForApplicationUpdates() const;
    /**
     * @brief Returns available application updates.
     *
     * This update set refers to the last update set retrieved on @ref lastCheckForUpdates.
     *
     * @returns A list of available application updates.
     *
     * @sa lastCheckForUpdates
     * @sa ApplicationUpdate
     */
    Hemera::SoftwareManagement::ApplicationUpdates availableApplicationUpdates() const;
    /**
     * @brief Returns all the applications installed on this system.
     *
     * This update refers to the last update set retrieved on @ref lastCheckForUpdates.
     *
     * @returns The installed applications.
     */
    Hemera::SoftwareManagement::ApplicationPackages installedApplications() const;

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
    Hemera::Operation *checkForApplicationUpdates();

    /**
     * @brief Downloads selected application updates.
     *
     * This operation downloads and caches to the system selected application updates, making them ready for installation.
     *
     * @note The update list must be consistent with available updates. If the list includes updates not available on
     *       the system, the method will return a null pointer.
     * @note The system might be configured to automatically download updates.
     * @note This method requires feature @e DownloadUpdates to be enabled for being executed successfully.
     *
     * @p updates A list of updates to download
     *
     * @returns An @ref Operation representing the update download, or a null pointer if @p updates is inconsistent.
     */
    Hemera::Operation *downloadApplicationUpdates(const Hemera::SoftwareManagement::ApplicationUpdates &updates);

    /**
     * @brief Updates selected applications
     *
     * This operation updates selected application. The operation will first download all the needed updates, and
     * install them afterwards. You don't need to call @ref downloadApplicationUpdates before this method. If
     * @ref downloadApplicationUpdates was called previously or the system is configured for automatically downloading
     * updates, the download phase will likely be skipped.
     *
     * @par Orbit restart
     *
     * Orbits affected by the update (as in: orbits running applications affected by the update) will be stopped before the
     * update procedure begins and restarted once the update procedure ends. Gravity will not change the active orbit in the
     * process, but will instead leave the handler with no active orbit during the update if the active orbit is affected by the update.
     *
     * If the orbit in which the application requesting the update lives is affected by the update, it will be restarted as well.
     * This won't influence the update process, which will continue as requested.
     *
     * This behavior, albeit discouraged, can be overridden on a per-application basis or on a system basis. Please see Gravity's
     * @ref applicationUpdatePolicy and application feature @ref DoNotRestartOnUpdate.
     *
     * @note The update list must be consistent with available updates. If the list includes updates not available on
     *       the system, the method will return a null pointer.
     * @note This method requires feature @e UpdateApplications to be enabled for being executed successfully.
     *
     * @p updates A list of updates to install
     *
     * @returns An @ref Operation representing the update procedure, or a null pointer if @p updates is inconsistent.
     */
    Hemera::Operation *updateApplications(const Hemera::SoftwareManagement::ApplicationUpdates &updates);
    
    Hemera::Operation *installApplications(const Hemera::SoftwareManagement::ApplicationPackages &applications);
    Hemera::Operation *removeApplications(const Hemera::SoftwareManagement::ApplicationPackages &applications);

Q_SIGNALS:
    /**
     * @brief Emitted when available system and/or application updates changed.
     */
    void availableApplicationUpdatesChanged();
    /**
     * @brief Emitted when the system is successfully checked for updates, and the last check occurrence changes.
     */
    void lastCheckForApplicationUpdatesChanged(const QDateTime &lastCheck);
    /**
     * @brief Emitted when applications are installed or removed, hence the installed applications on the system change.
     */
    void installedApplicationsChanged();

private:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

};
}

}

#endif // HEMERA_APPLICATIONMANAGER_H
