#ifndef HEMERA_OPERATIONWITHPROGRESS_H
#define HEMERA_OPERATIONWITHPROGRESS_H

#include <HemeraCore/Operation>

namespace Hemera {

class OperationWithProgressPrivate;
/**
 * @class OperationWithProgress
 * @ingroup HemeraCore
 * @headerfile Hemera/hemeraoperationwithprogress.h <HemeraCore/OperationWithProgress>
 *
 * @brief A pending asynchronous operation with a progress interface.
 *
 * OperationWithProgress adds a basic progress interface to Operation, allowing to report
 * a progress percentage and a status message.
 *
 * @sa Operation
 */
class HEMERA_QT5_SDK_EXPORT OperationWithProgress : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(OperationWithProgress)
    Q_DECLARE_PRIVATE(OperationWithProgress)

    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    /**
     * Default destructor
     */
    virtual ~OperationWithProgress();

    /**
     * @returns The progress of the operation
     *
     * @note This method will return a meaningful value only when the Operation has been started.
     */
    int progress() const;
    /**
     * @returns The status message of the operation
     *
     * @note This method will return a meaningful value only when the Operation has been started.
     */
    QString statusMessage() const;

Q_SIGNALS:
    /**
     * @brief Emitted when the Operation's progress has changed.
     *
     * @param progress The new progress of the operation.
     */
    void progressChanged(int progress);
    /**
     * @brief Emitted when the Operation's status message has changed.
     *
     * @param message The new status message of the operation.
     */
    void statusMessageChanged(const QString &message);

protected:
    explicit OperationWithProgress(QObject *parent = nullptr);
    explicit OperationWithProgress(ExecutionOptions options, QObject *parent = nullptr);
    explicit OperationWithProgress(OperationWithProgressPrivate &dd, ExecutionOptions options, QObject *parent = nullptr);

    /**
     * @brief Sets the new progress of the operation.
     *
     * @note Subclasses should call this method, and not emit progressChanged directly.
     *
     * @param progress The new progress of the operation.
     */
    void setProgress(int progress);
    /**
     * @brief Sets the new status message of the operation.
     *
     * @note Subclasses should call this method, and not emit statusMessageChanged directly.
     *
     * @param message The new status message of the operation.
     */
    void setStatusMessage(const QString &message);
};
}

#endif // HEMERA_OPERATIONWITHPROGRESS_H
