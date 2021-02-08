#include "hemeracxxsoftwaremanagementsoftwaremanager.h"

#include "hemeracxxsoftwaremanagementapplicationpackage_p.h"
#include "hemeracxxsoftwaremanagementapplicationupdate_p.h"

#include <QtCore/QDebug>
#include <QtCore/QObject>

#include <HemeraCore/Operation>
#include <HemeraCore/CommonOperations>

#include <HemeraSoftwareManagement/ProgressReporter>
#include <HemeraSoftwareManagement/SoftwareManager>

namespace HemeraCxx {
namespace SoftwareManagement {

class SoftwareManagerPrivate : public QObject
{
    Q_OBJECT
public:
    SoftwareManagerPrivate(SoftwareManager *q) : q(q), progressCallback(nullptr) {}

    SoftwareManager *q;

    Hemera::SoftwareManagement::SoftwareManager *proxy;
    Hemera::SoftwareManagement::ProgressReporter *reporter;

    SoftwareManager::_progress_cb progressCallback;

    void performOperationWithCallbacks(Hemera::Operation *op, SoftwareManager::_progress_cb callback, void* callback_data,
                                      SoftwareManager::_finished_cb finished_callback, void* finished_callback_data);

public Q_SLOTS:
    void onOperationTypeChanged() { sendProgressToCallback(); }
    void onCurrentStepChanged() { sendProgressToCallback(); }
    void onDescriptionChanged() { sendProgressToCallback(); }
    void onProgressChanged() { sendProgressToCallback(); }

    void sendProgressToCallback();

private:
    void *progressCallbackData;
};

void SoftwareManagerPrivate::performOperationWithCallbacks(Hemera::Operation* operation, SoftwareManager::_progress_cb callback, void* callback_data,
                                                           SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    progressCallback = callback;
    progressCallbackData = callback_data;

    QObject::connect(operation, &Hemera::Operation::finished,
                     [this, finished_callback, finished_callback_data] (Hemera::Operation *op) {
        progressCallback = nullptr;
        progressCallbackData = nullptr;
        finished_callback(!op->isError(), finished_callback_data);
    });
}

void SoftwareManagerPrivate::sendProgressToCallback()
{
    if (progressCallback != nullptr) {
        progressCallback(static_cast<SoftwareManager::OperationType>(reporter->operationType()), static_cast<SoftwareManager::OperationStep>(reporter->currentStep()),
                         reporter->description().toStdString(), reporter->percent(), reporter->rate(), progressCallbackData);
    }
}

class SoftwareManagerHelper
{
public:
    SoftwareManagerHelper() : q(nullptr) {}
    ~SoftwareManagerHelper() {
        // Do not delete - the object will be cleaned up already.
    }
    SoftwareManager *q;
};

Q_GLOBAL_STATIC(SoftwareManagerHelper, s_globalCxxSoftwareManager)

SoftwareManager *SoftwareManager::softwareManagerInstance()
{
    if (!s_globalCxxSoftwareManager()->q) {
        // Create a new instance
        new SoftwareManager;
    }

    return s_globalCxxSoftwareManager()->q;
}

SoftwareManager::SoftwareManager()
    :d(new SoftwareManagerPrivate(this))
{
    Q_ASSERT(!s_globalCxxSoftwareManager()->q);
    s_globalCxxSoftwareManager()->q = this;

    d->proxy = new Hemera::SoftwareManagement::SoftwareManager();
    d->reporter = new Hemera::SoftwareManagement::ProgressReporter();

    // Connect signals and stuff
    QObject::connect(d->reporter, &Hemera::SoftwareManagement::ProgressReporter::operationTypeChanged, d, &SoftwareManagerPrivate::onOperationTypeChanged);
    QObject::connect(d->reporter, &Hemera::SoftwareManagement::ProgressReporter::currentStepChanged, d, &SoftwareManagerPrivate::onCurrentStepChanged);
    QObject::connect(d->reporter, &Hemera::SoftwareManagement::ProgressReporter::descriptionChanged, d, &SoftwareManagerPrivate::onDescriptionChanged);
    QObject::connect(d->reporter, &Hemera::SoftwareManagement::ProgressReporter::progressChanged, d, &SoftwareManagerPrivate::onProgressChanged);

    // Perform initialization of the proxies straight away, synchronized.
    QList< Hemera::Operation* > operations;
    operations.append(d->proxy->init());
    operations.append(d->reporter->init());
    Hemera::CompositeOperation *operation = new Hemera::CompositeOperation(operations);
    operation->synchronize();
}

SoftwareManager::~SoftwareManager()
{
    s_globalCxxSoftwareManager()->q = nullptr;

    d->reporter->deleteLater();
    d->proxy->deleteLater();

    delete d;
}

ApplicationUpdates SoftwareManager::availableApplicationUpdates() const
{
    return ApplicationUpdateData::toCxx(d->proxy->availableApplicationUpdates());
}

SystemUpdate SoftwareManager::availableSystemUpdate() const
{
    return SystemUpdate(d->proxy->availableSystemUpdate());
}

void SoftwareManager::checkForUpdates(SoftwareManager::_progress_cb callback, void* callback_data,
                                      SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->checkForUpdates(), callback, callback_data, finished_callback, finished_callback_data);
}

void SoftwareManager::downloadApplicationUpdates(const ApplicationUpdates &updates, SoftwareManager::_progress_cb callback,
                                                 void* callback_data, SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->downloadApplicationUpdates(ApplicationUpdateData::fromCxx(updates)),
                                     callback, callback_data, finished_callback, finished_callback_data);
}

void SoftwareManager::downloadSystemUpdate(SoftwareManager::_progress_cb callback, void* callback_data,
                                           SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->downloadSystemUpdate(), callback, callback_data, finished_callback, finished_callback_data);
}

void SoftwareManager::installApplications(const ApplicationPackages& applications, SoftwareManager::_progress_cb callback,
                                          void* callback_data, SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->installApplications(ApplicationPackageData::fromCxx(applications)),
                                     callback, callback_data, finished_callback, finished_callback_data);
}

ApplicationPackages SoftwareManager::installedApplications() const
{
    return ApplicationPackageData::toCxx(d->proxy->installedApplications());
}

long long unsigned int SoftwareManager::lastCheckForUpdates() const
{
    return d->proxy->lastCheckForUpdates().currentMSecsSinceEpoch();
}

void SoftwareManager::removeApplications(const ApplicationPackages& applications, SoftwareManager::_progress_cb callback, void* callback_data,
                                         SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->removeApplications(ApplicationPackageData::fromCxx(applications)),
                                     callback, callback_data, finished_callback, finished_callback_data);
}

void SoftwareManager::updateApplications(const ApplicationUpdates& updates, SoftwareManager::_progress_cb callback, void* callback_data,
                                         SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->updateApplications(ApplicationUpdateData::fromCxx(updates)),
                                     callback, callback_data, finished_callback, finished_callback_data);
}

void SoftwareManager::updateSystem(SoftwareManager::_progress_cb callback, void* callback_data,
                                   SoftwareManager::_finished_cb finished_callback, void* finished_callback_data)
{
    if (d->progressCallback != nullptr) {
        // Something is going on, refuse to proceed
        qWarning() << "Tried to perform an operation, but another one is going on already!";
    }

    d->performOperationWithCallbacks(d->proxy->updateSystem(), callback, callback_data, finished_callback, finished_callback_data);
}

}
}

#include "hemeracxxsoftwaremanagementsoftwaremanager.moc"
