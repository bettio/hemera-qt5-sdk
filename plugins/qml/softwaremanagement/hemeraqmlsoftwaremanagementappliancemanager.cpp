/*
 *
 */

#include "hemeraqmlsoftwaremanagementappliancemanager.h"

#include <QtCore/QLoggingCategory>

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

#include <HemeraSoftwareManagement/ApplianceManager>
#include <HemeraSoftwareManagement/SystemUpdate>

#include "hemeraqmlsoftwaremanagementsystemupdate.h"


Q_LOGGING_CATEGORY(LOG_HEMERA_QMLAPPLIANCEMANAGER, "Hemera::Qml::SoftwareManagement::ApplianceManager")

namespace Hemera {

namespace Qml {

namespace SoftwareManagement {

class ApplianceManager::Private
{
public:
    Private(ApplianceManager *q) : q(q) {}
    virtual ~Private() {}

    ApplianceManager *q;

    QString lastErrorName;
    QString lastErrorMessage;

    SystemUpdate *proxySystemUpdate;
    Hemera::SoftwareManagement::ApplianceManager *proxyApplianceManager;

    bool checkingUpdates;
};


ApplianceManager::ApplianceManager(QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->checkingUpdates = false;
    // Create our proxy ApplianceManager and SystemUpdate
    d->proxySystemUpdate = new SystemUpdate(this);
    d->proxyApplianceManager = new Hemera::SoftwareManagement::ApplianceManager(this);
    connect(d->proxyApplianceManager->init(), &Hemera::Operation::finished, this, [this] (Hemera::Operation *op) {
        if (op->isError()) {
            d->lastErrorName = op->errorName();
            d->lastErrorMessage = op->errorMessage();
            Q_EMIT error();
        } else {
            d->lastErrorName.clear();
            d->lastErrorMessage.clear();
        }
    });

    // Connect proxy signals
    connect(d->proxyApplianceManager, &Hemera::SoftwareManagement::ApplianceManager::availableSystemUpdateChanged, this, [this] {
        d->proxySystemUpdate->setSystemUpdate(d->proxyApplianceManager->availableSystemUpdate());
        Q_EMIT availableSystemUpdateChanged();
    });
    connect(d->proxyApplianceManager, &Hemera::SoftwareManagement::ApplianceManager::lastCheckForUpdatesChanged,
            this, &ApplianceManager::lastCheckForUpdatesChanged);
    connect(d->proxyApplianceManager, &Hemera::SoftwareManagement::ApplianceManager::applianceVersionChanged,
            this, &ApplianceManager::applianceVersionChanged);
    connect(d->proxyApplianceManager, &Hemera::SoftwareManagement::ApplianceManager::hemeraVersionChanged,
            this, &ApplianceManager::hemeraVersionChanged);
}

ApplianceManager::~ApplianceManager()
{
    delete d;
}

SystemUpdate *ApplianceManager::availableSystemUpdate() const
{
    return d->proxySystemUpdate;
}

QDateTime ApplianceManager::lastCheckForUpdates() const
{
    return d->proxyApplianceManager->lastCheckForUpdates();
}

bool ApplianceManager::isCheckingForUpdates() const
{
    return d->checkingUpdates;
}

QString ApplianceManager::applianceName() const
{
    return d->proxyApplianceManager->applianceName();
}

QString ApplianceManager::applianceVariant() const
{
    return d->proxyApplianceManager->applianceVariant();
}

QString ApplianceManager::applianceVersion() const
{
    return d->proxyApplianceManager->applianceVersion();
}

QString ApplianceManager::hemeraVersion() const
{
    return d->proxyApplianceManager->hemeraVersion();
}

QString ApplianceManager::errorName() const
{
    return d->lastErrorName;
}

QString ApplianceManager::errorMessage() const
{
    return d->lastErrorMessage;
}

void ApplianceManager::checkForUpdates(Hemera::SoftwareManagement::SystemUpdate::UpdateType preferredUpdateType)
{
    if (Q_UNLIKELY(!d->proxyApplianceManager->isReady())) {
        connect(d->proxyApplianceManager, &Hemera::AsyncInitObject::ready, this, [this, preferredUpdateType] {
            checkForUpdates(preferredUpdateType);
        });
        return;
    }

    d->checkingUpdates = true;
    Q_EMIT checkingForUpdatesChanged();

    qWarning() << "Check for updates " << (quint16) preferredUpdateType;
    connect(d->proxyApplianceManager->checkForUpdates(preferredUpdateType), &Hemera::Operation::finished, this, [this] (Hemera::Operation *op) {
        if (op->isError()) {
            d->lastErrorName = op->errorName();
            d->lastErrorMessage = op->errorMessage();
            Q_EMIT error();
        } else {
            d->lastErrorName.clear();
            d->lastErrorMessage.clear();
        }

        d->checkingUpdates = false;
        Q_EMIT checkingForUpdatesChanged();
        Q_EMIT checkForUpdatesFinished();
    });
}

void ApplianceManager::downloadSystemUpdate()
{
    if (Q_UNLIKELY(!d->proxyApplianceManager->isReady())) {
        connect(d->proxyApplianceManager, &Hemera::AsyncInitObject::ready, this, [this] {
            downloadSystemUpdate();
        });
        return;
    }

    connect(d->proxyApplianceManager->downloadSystemUpdate(), &Hemera::Operation::finished, this, [this] (Hemera::Operation *op) {
        if (op->isError()) {
            d->lastErrorName = op->errorName();
            d->lastErrorMessage = op->errorMessage();
            Q_EMIT error();
        } else {
            d->lastErrorName.clear();
            d->lastErrorMessage.clear();
        }

        Q_EMIT downloadSystemUpdateFinished();
    });
}

void ApplianceManager::updateSystem()
{
    if (Q_UNLIKELY(!d->proxyApplianceManager->isReady())) {
        connect(d->proxyApplianceManager, &Hemera::AsyncInitObject::ready, this, [this] {
            updateSystem();
        });
        return;
    }

    connect(d->proxyApplianceManager->updateSystem(), &Hemera::Operation::finished, this, [this] (Hemera::Operation *op) {
        if (op->isError()) {
            d->lastErrorName = op->errorName();
            d->lastErrorMessage = op->errorMessage();
            Q_EMIT error();
        } else {
            d->lastErrorName.clear();
            d->lastErrorMessage.clear();
        }

        Q_EMIT updateSystemFinished();
    });
}

}

}

}
