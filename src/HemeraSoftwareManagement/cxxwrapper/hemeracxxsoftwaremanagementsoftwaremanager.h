#ifndef HEMERACXX_SOFTWAREMANAGEMENT_SOFTWAREMANAGER_H
#define HEMERACXX_SOFTWAREMANAGEMENT_SOFTWAREMANAGER_H

#include <HemeraCore/Global>
#include <HemeraCxxCore/Global>

#include <HemeraCxxSoftwareManagement/ApplicationPackage>
#include <HemeraCxxSoftwareManagement/ApplicationUpdate>
#include <HemeraCxxSoftwareManagement/SystemUpdate>

namespace HemeraCxx {
namespace SoftwareManagement {

class SoftwareManagerPrivate;
class SoftwareManager
{
public:
    enum class OperationType {
        None = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_None,
        RefreshRepositories = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_RefreshRepositories,
        UpdateApplications = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_UpdateApplications,
        UpdateSystem = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_UpdateSystem,
        InstallApplications = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_InstallApplications,
        RemoveApplications = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_RemoveApplications
    };
    enum class OperationStep {
        NoStep = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_NoStep,
        Download = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Download,
        Process = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Process,
        Cleanup = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Cleanup
    };

    SoftwareManager();
    virtual ~SoftwareManager();

    static SoftwareManager *softwareManagerInstance();

    unsigned long long lastCheckForUpdates() const;
    HemeraCxx::SoftwareManagement::ApplicationUpdates availableApplicationUpdates() const;
    HemeraCxx::SoftwareManagement::SystemUpdate availableSystemUpdate() const;
    HemeraCxx::SoftwareManagement::ApplicationPackages installedApplications() const;

    typedef void (*_progress_cb)(SoftwareManager::OperationType type,SoftwareManager::OperationStep step,
                                 std::string description,int progress,int rate,void* data);
    typedef void (*_finished_cb)(bool result,void*);

    void checkForUpdates(_progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);

    void downloadApplicationUpdates(const HemeraCxx::SoftwareManagement::ApplicationUpdates &updates,
                                    _progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);
    void downloadSystemUpdate(_progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);

    void updateApplications(const HemeraCxx::SoftwareManagement::ApplicationUpdates &updates,
                            _progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);
    void updateSystem(_progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);

    void installApplications(const HemeraCxx::SoftwareManagement::ApplicationPackages &applications,
                             _progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);
    void removeApplications(const HemeraCxx::SoftwareManagement::ApplicationPackages &applications,
                            _progress_cb callback, void *callback_data, _finished_cb finished_callback, void *finished_callback_data);

private:
    SoftwareManagerPrivate * const d;
};

// Wrappers
typedef SoftwareManager SoftwareManagerWrapper;

}
}

#endif // HEMERACXX_SOFTWAREMANAGEMENT_SOFTWAREMANAGER_H
