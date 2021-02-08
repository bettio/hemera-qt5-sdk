#include "hemerasoftwaremanagementprogressreporter.h"

#include "hemeraasyncinitobject_p.h"

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

#include "progressreporterinterface.h"
#include "dbusobjectinterface.h"

Q_LOGGING_CATEGORY(LOG_HEMERA_SOFTWAREMANAGERPROGRESSREPORTER, "Hemera::SoftwareManagement::ProgressReporter")

namespace Hemera {

namespace SoftwareManagement {

class ProgressReporterPrivate : public AsyncInitObjectPrivate
{
public:
    ProgressReporterPrivate(ProgressReporter *q)
        : AsyncInitObjectPrivate(q)
        , progressReporterInterface(nullptr)
        , operationType(Hemera::SoftwareManagement::ProgressReporter::OperationType::None)
        , availableSteps(Hemera::SoftwareManagement::ProgressReporter::OperationStep::NoStep)
        , currentStep(ProgressReporter::OperationStep::NoStep)
        , percent(0)
        , rate(0) {}
    virtual ~ProgressReporterPrivate() {}

    Q_DECLARE_PUBLIC(ProgressReporter)

    virtual void initHook() Q_DECL_OVERRIDE;

    com::ispirata::Hemera::SoftwareManagement::ProgressReporter *progressReporterInterface;
    com::ispirata::Hemera::DBusObject *progressReporterObjectInterface;

    QByteArray operationId;
    QDateTime startDateTime;
    ProgressReporter::OperationType operationType;
    ProgressReporter::OperationSteps availableSteps;
    ProgressReporter::OperationStep currentStep;

    QString description;
    int percent;
    int rate;
};

void ProgressReporterPrivate::initHook()
{
    Hemera::AsyncInitObjectPrivate::initHook();
}


ProgressReporter::ProgressReporter(QObject* parent)
    : AsyncInitObject(*new ProgressReporterPrivate(this), parent)
{
}

ProgressReporter::~ProgressReporter()
{
    // Unsubscribe
    Q_D(ProgressReporter);

    if (d->progressReporterInterface != nullptr) {
        // There's worse things than deadlock.
        QDBusPendingReply<void> reply = d->progressReporterInterface->unsubscribe();
        reply.waitForFinished();
        if (reply.isError()) {
            qCWarning(LOG_HEMERA_SOFTWAREMANAGERPROGRESSREPORTER) << tr("Could not unsubscribe from progress reporter. Hope we'll be garbage collected!");
        }
    }
}

void ProgressReporter::initImpl()
{
    // First of all, we want to check whether our service is up.
    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(Literals::literal(Literals::DBus::softwareManagerService()))) {
        // Houston, we have a problem
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("There's no software manager on this Hemera system, or the software manager failed on startup."));
        return;
    }

    Q_D(ProgressReporter);

    d->progressReporterInterface = new com::ispirata::Hemera::SoftwareManagement::ProgressReporter(
                                                                Literals::literal(Literals::DBus::softwareManagerService()),
                                                                Literals::literal(Literals::DBus::softwareManagementProgressReporterPath()),
                                                                QDBusConnection::systemBus(), this);

    if (!d->progressReporterInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus interface is not available. The software manager has probably encountered a failure."));
        return;
    }

    d->progressReporterObjectInterface = new com::ispirata::Hemera::DBusObject(Literals::literal(Literals::DBus::softwareManagerService()),
                                                                               Literals::literal(Literals::DBus::softwareManagementProgressReporterPath()),
                                                                               QDBusConnection::systemBus(), this);

    if (!d->progressReporterObjectInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus object interface is not available. The software manager has probably encountered a failure."));
        return;
    }

    setParts(3);

    // Subscribe, first of all
    DBusVoidOperation *op = new DBusVoidOperation(d->progressReporterInterface->subscribe(), this);
    connect(op, &Operation::finished, [this, d, op] {
        if (op->isError()) {
            setInitError(op->errorName(), op->errorMessage());
            return;
        }

        // Ok.
        setOnePartIsReady();
    });

    // Cache properties and connect to signals
    DBusVariantMapOperation *operation = new DBusVariantMapOperation(d->progressReporterObjectInterface->allProperties(), this);
    connect(operation, &Operation::finished, this, [this, d, operation] {
        if (operation->isError()) {
            setInitError(operation->errorName(), operation->errorMessage());
            return;
        }

        // Set the various properties
        QVariantMap result = operation->result();
        if (!result.value(QStringLiteral("initError")).toString().isEmpty()) {
            // The software manager has failed.
            setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                         tr("The software manager encountered an error: %1, %2").arg(result.value(QStringLiteral("initError")).toString(),
                                                                                     result.value(QStringLiteral("initErrorMessage")).toString()));
            return;
        }

        d->operationId = result.value(QStringLiteral("operationId")).toByteArray();
        d->startDateTime = QDateTime::fromMSecsSinceEpoch(result.value(QStringLiteral("startDateTime"), 0).toLongLong());
        d->operationType = static_cast<OperationType>(result.value(QStringLiteral("operationType"), 0).toUInt());
        d->availableSteps = static_cast<OperationSteps>(result.value(QStringLiteral("availableSteps"), 0).toUInt());
        d->currentStep = static_cast<OperationStep>(result.value(QStringLiteral("currentStep"), 0).toUInt());

        d->description = result.value(QStringLiteral("description")).toString();
        d->percent = result.value(QStringLiteral("percent"), 0).toInt();
        d->rate = result.value(QStringLiteral("rate"), 0).toInt();

        setOnePartIsReady();
    });

    connect(d->progressReporterObjectInterface, &com::ispirata::Hemera::DBusObject::propertiesChanged, this, [this, d] (const QVariantMap &changed) {
        bool isOperationTypeChanged = false;
        if (changed.contains(QStringLiteral("operationId"))) {
            d->operationId = changed.value(QStringLiteral("operationId")).toByteArray();
            isOperationTypeChanged = true;
        }
        if (changed.contains(QStringLiteral("startDateTime"))) {
            d->startDateTime = QDateTime::fromMSecsSinceEpoch(changed.value(QStringLiteral("startDateTime")).toLongLong());
            isOperationTypeChanged = true;
        }
        if (changed.contains(QStringLiteral("operationType"))) {
            d->operationType = static_cast<OperationType>(changed.value(QStringLiteral("operationType")).toUInt());
            isOperationTypeChanged = true;
        }
        if (changed.contains(QStringLiteral("availableSteps"))) {
            d->availableSteps = static_cast<OperationSteps>(changed.value(QStringLiteral("availableSteps"), 0).toUInt());
            isOperationTypeChanged = true;
        }
        if (isOperationTypeChanged) {
            Q_EMIT operationTypeChanged(d->operationType);
        }

        if (changed.contains(QStringLiteral("currentStep"))) {
            d->currentStep = static_cast<OperationStep>(changed.value(QStringLiteral("currentStep")).toUInt());
            Q_EMIT currentStepChanged(d->currentStep);
        }
        if (changed.contains(QStringLiteral("description"))) {
            d->description = changed.value(QStringLiteral("description")).toString();
            Q_EMIT descriptionChanged(d->description);
        }

        bool isProgressChanged = false;
        if (changed.contains(QStringLiteral("percent"))) {
            d->percent = changed.value(QStringLiteral("percent")).toInt();
            isProgressChanged = true;
        }
        if (changed.contains(QStringLiteral("rate"))) {
            d->rate = changed.value(QStringLiteral("rate")).toInt();
            isProgressChanged = true;
        }
        if (isProgressChanged) {
            Q_EMIT progressChanged();
        }
    });

    setOnePartIsReady();
}

ProgressReporter::OperationSteps ProgressReporter::availableSteps() const
{
    Q_D(const ProgressReporter);
    return d->availableSteps;
}

QByteArray ProgressReporter::operationId() const
{
    Q_D(const ProgressReporter);
    return d->operationId;
}

QDateTime ProgressReporter::startDateTime() const
{
    Q_D(const ProgressReporter);
    return d->startDateTime;
}

ProgressReporter::OperationType ProgressReporter::operationType() const
{
    Q_D(const ProgressReporter);
    return d->operationType;
}

ProgressReporter::OperationStep ProgressReporter::currentStep() const
{
    Q_D(const ProgressReporter);
    return d->currentStep;
}

QString ProgressReporter::description() const
{
    Q_D(const ProgressReporter);
    return d->description;
}

int ProgressReporter::percent() const
{
    Q_D(const ProgressReporter);
    return d->percent;
}

int ProgressReporter::rate() const
{
    Q_D(const ProgressReporter);
    return d->rate;
}

}

}
