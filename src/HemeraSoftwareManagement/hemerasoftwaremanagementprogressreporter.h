#ifndef HEMERA_SOFTWAREMANAGERPROGRESSREPORTER_H
#define HEMERA_SOFTWAREMANAGERPROGRESSREPORTER_H

#include <HemeraCore/AsyncInitObject>

#include <QtCore/QDateTime>

namespace Hemera {

namespace SoftwareManagement {

class ProgressReporterPrivate;
/**
 * @class ProgressReporter
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagerprogressreporter.h <HemeraSoftwareManagement/ProgressReporter>
 *
 * @brief Provides progress information over Software Management operation.
 *
 * ProgressReporter is a read-only class which allows to monitor any Software Management operation
 * happening in the system. This means that such a class monitors every operation, not only those originating
 * from SoftwareManagers in its same application.
 *
 * Due to the nature of the software manager, only one operation at a time can be performed.
 *
 * Usually, only one single ProgressReporter object should be instantiated for each application for performance reasons.
 *
 * @sa SoftwareManager
 */
class ProgressReporter : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ProgressReporter)
    Q_DECLARE_PRIVATE_D(d_h_ptr, ProgressReporter)

    /// The operation ID of the current operation, or an empty byte array if no operation is in progress.
    Q_PROPERTY(QByteArray operationId READ operationId NOTIFY operationTypeChanged)
    /// The start date and time of the current operation, or an invalid datetime object if no operation is in progress.
    Q_PROPERTY(QDateTime startDateTime READ startDateTime NOTIFY operationTypeChanged)
    /// The operation type of the current operation, or @e None if no operation is in progress.
    Q_PROPERTY(Hemera::SoftwareManagement::ProgressReporter::OperationType operationType READ operationType NOTIFY operationTypeChanged)
    /// Available steps of the current operation, or @e NoStep if no operation is in progress.
    Q_PROPERTY(Hemera::SoftwareManagement::ProgressReporter::OperationSteps availableSteps READ availableSteps NOTIFY operationTypeChanged)

    /// The current step of the current operation, or @e NoStep if no operation is in progress.
    Q_PROPERTY(Hemera::SoftwareManagement::ProgressReporter::OperationStep currentStep READ currentStep NOTIFY currentStepChanged)
    /// The description of the current action of the current operation, or an empty string if no operation is in progress.
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    /// The completion percentage of the current step of the current operation, or @e NoStep if no operation is in progress.
    Q_PROPERTY(int percent READ percent NOTIFY progressChanged)
    /// The progress rate, in B/s, of the current operation. @sa rate()
    Q_PROPERTY(int rate READ rate NOTIFY progressChanged)

public:
    /**
     * Defines the type of the operation currently in progress.
     */
    enum class OperationType {
        /// No operation is being performed
        None = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_None,
        /// Repositories are being refreshed
        RefreshRepositories = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_RefreshRepositories,
        /// Applications are being updated
        UpdateApplications = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_UpdateApplications,
        /// System is being updated
        UpdateSystem = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_UpdateSystem,
        /// New Applications are being installed
        InstallApplications = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_InstallApplications,
        /// Applications are being removed
        RemoveApplications = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_RemoveApplications
    };
    Q_ENUM(OperationType)

    /**
     * Defines a step in an operation. Operations can have more than one step, depending on their nature.
     */
    enum class OperationStep {
        /// Invalid step
        NoStep = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_NoStep,
        /// Download payloads from a remote source
        Download = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Download,
        /// Act on the filesystem
        Process = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Process,
        /// Cleanup temporary files or actions
        Cleanup = HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Cleanup
    };
    Q_ENUM(OperationStep)
    Q_DECLARE_FLAGS(OperationSteps, OperationStep)

    explicit ProgressReporter(QObject* parent = nullptr);
    virtual ~ProgressReporter();

    /**
     * @brief The operation ID of the current operation
     *
     * @returns The operation ID of the current operation, or an empty byte array if no operation is in progress.
     */
    QByteArray operationId() const;
    /**
     * @brief The start date and time of the current operation.
     *
     * @returns The start date and time of the current operation, or an invalid datetime object if no operation is in progress.
     */
    QDateTime startDateTime() const;
    /**
     * @brief The operation type of the current operation
     *
     * @returns The operation type of the current operation, or @e None if no operation is in progress.
     */
    OperationType operationType() const;
    /**
     * @brief Available steps of the current operation
     *
     * @returns Available steps of the current operation, or @e NoStep if no operation is in progress.
     */
    OperationSteps availableSteps() const;
    /**
     * @brief The current step of the current operation
     *
     * @returns The current step of the current operation, or @e NoStep if no operation is in progress.
     */
    OperationStep currentStep() const;

    /**
     * @brief The description of the current action of the current operation
     *
     * @returns The description of the current action of the current operation, or an empty string if no operation is in progress.
     */
    QString description() const;
    /**
     * @brief The completion percentage of the current step of the current operation
     *
     * @returns The completion percentage of the current step of the current operation, or @e NoStep if no operation is in progress.
     */
    int percent() const;
    /**
     * @brief The progress rate, in B/s, of the current operation
     *
     * @returns The progress rate, in B/s, of the current operation.
     *
     * @note If no operation is in progress, this function will return an invalid value.
     * @note This function will return 0 if the current step does not support rate report. Usually, only @e Download supports this feature.
     */
    int rate() const;

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

Q_SIGNALS:
    /**
     * @brief Emitted when the operation type changes
     */
    void operationTypeChanged(Hemera::SoftwareManagement::ProgressReporter::OperationType type);
    /**
     * @brief Emitted when the current step changes
     */
    void currentStepChanged(Hemera::SoftwareManagement::ProgressReporter::OperationStep step);
    /**
     * @brief Emitted when the description changes
     */
    void descriptionChanged(const QString &description);
    /**
     * @brief Emitted when the progress (percent, rate or both) changes
     */
    void progressChanged();
};

}

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Hemera::SoftwareManagement::ProgressReporter::OperationSteps)

Q_DECLARE_METATYPE(Hemera::SoftwareManagement::ProgressReporter::OperationStep)
Q_DECLARE_METATYPE(Hemera::SoftwareManagement::ProgressReporter::OperationSteps)
Q_DECLARE_METATYPE(Hemera::SoftwareManagement::ProgressReporter::OperationType)

#endif // HEMERA_SOFTWAREMANAGERPROGRESSREPORTER_H
