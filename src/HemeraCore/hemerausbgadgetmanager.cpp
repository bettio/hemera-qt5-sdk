#include "hemerausbgadgetmanager.h"

#include "hemeraasyncinitobject_p.h"
#include "hemeracommonoperations.h"

#include <HemeraCore/Literals>

#include <usbgadgetmanagerinterface.h>
#include <dbusobjectinterface.h>

namespace Hemera {

class USBGadgetManagerPrivate : public AsyncInitObjectPrivate
{
public:
    USBGadgetManagerPrivate(USBGadgetManager *q)
        : AsyncInitObjectPrivate(q) {}

    com::ispirata::Hemera::USBGadgetManager *interface;
    com::ispirata::Hemera::DBusObject *dbusObject;

    USBGadgetManager::Modes availableModes;
    USBGadgetManager::Mode activeMode;

    bool canDetectCableHotplugging;
    USBGadgetManager::USBCableStatus usbCableStatus;

    bool systemWideLockActive;
    QString systemWideLockOwner;
    QString systemWideLockReason;
};

USBGadgetManager::USBGadgetManager(QObject* parent)
    : AsyncInitObject(*new USBGadgetManagerPrivate(this), parent)
{

}

USBGadgetManager::~USBGadgetManager()
{
}

void USBGadgetManager::initImpl()
{
    Q_D(USBGadgetManager);

    d->interface = new com::ispirata::Hemera::USBGadgetManager(Literals::literal(Literals::DBus::usbGadgetManagerService()),
                                                               Literals::literal(Literals::DBus::usbGadgetManagerPath()),
                                                               QDBusConnection::systemBus(), this);

    if (!d->interface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus interface is not available. The daemon is probably not running."));
        return;
    }

    d->dbusObject = new com::ispirata::Hemera::DBusObject(Literals::literal(Literals::DBus::usbGadgetManagerService()),
                                                          Literals::literal(Literals::DBus::usbGadgetManagerPath()),
                                                          QDBusConnection::systemBus(), this);

    if (!d->dbusObject->isValid()) {
        setInitError(Literals::literal(Literals::Errors::dbusObjectNotAvailable()),
                     tr("The remote DBus object is not available. The daemon is probably not running."));
        return;
    }

    setParts(2);

    Hemera::DBusVariantMapOperation *operation = new DBusVariantMapOperation(d->dbusObject->allProperties(), this);
    connect(operation, &Operation::finished, [this, operation, d] {
        if (operation->isError()) {
            setInitError(operation->errorName(), operation->errorMessage());
            return;
        }

        // Set the various properties
        QVariantMap result = operation->result();
        d->activeMode = static_cast< USBGadgetManager::Mode >(result.value(QStringLiteral("activeMode")).toUInt());
        d->availableModes = static_cast< USBGadgetManager::Modes >(result.value(QStringLiteral("availableModes")).toUInt());
        d->usbCableStatus = static_cast< USBGadgetManager::USBCableStatus >(result.value(QStringLiteral("usbCableStatus")).toUInt());
        d->canDetectCableHotplugging = result.value(QStringLiteral("canDetectCableHotplugging")).toBool();
        d->systemWideLockActive = result.value(QStringLiteral("systemWideLockActive")).toBool();
        d->systemWideLockOwner = result.value(QStringLiteral("systemWideLockOwner")).toString();
        d->systemWideLockReason = result.value(QStringLiteral("systemWideLockReason")).toString();

        setOnePartIsReady();
    });

    connect(d->dbusObject, &com::ispirata::Hemera::DBusObject::propertiesChanged, [this, d] (const QVariantMap &changed) {
        if (changed.contains(QStringLiteral("activeMode"))) {
            d->activeMode = static_cast< USBGadgetManager::Mode >(changed.value(QStringLiteral("activeMode")).toUInt());
            Q_EMIT activeModeChanged(d->activeMode);
        }
        if (changed.contains(QStringLiteral("systemWideLockActive"))) {
            d->systemWideLockActive = changed.value(QStringLiteral("systemWideLockActive")).toBool();
            Q_EMIT systemWideLockChanged(d->systemWideLockActive);
        }
        if (changed.contains(QStringLiteral("usbCableStatus"))) {
            d->usbCableStatus = static_cast< USBGadgetManager::USBCableStatus >(changed.value(QStringLiteral("usbCableStatus")).toUInt());
            Q_EMIT usbCableStatusChanged(d->usbCableStatus);
        }
        if (changed.contains(QStringLiteral("canDetectCableHotplugging"))) {
            d->canDetectCableHotplugging = changed.value(QStringLiteral("canDetectCableHotplugging")).toBool();
        }
        if (changed.contains(QStringLiteral("availableModes"))) {
            d->availableModes = static_cast< USBGadgetManager::Modes >(changed.value(QStringLiteral("availableModes")).toUInt());
        }
        if (changed.contains(QStringLiteral("systemWideLockOwner"))) {
            d->systemWideLockOwner = changed.value(QStringLiteral("systemWideLockOwner")).toString();
        }
        if (changed.contains(QStringLiteral("systemWideLockReason"))) {
            d->systemWideLockReason = changed.value(QStringLiteral("systemWideLockReason")).toString();
        }
    });

    setOnePartIsReady();
}

USBGadgetManager::Mode USBGadgetManager::activeMode() const
{
    Q_D(const USBGadgetManager);
    return d->activeMode;
}

USBGadgetManager::Modes USBGadgetManager::availableModes() const
{
    Q_D(const USBGadgetManager);
    return d->availableModes;
}

USBGadgetManager::USBCableStatus USBGadgetManager::usbCableStatus() const
{
    Q_D(const USBGadgetManager);
    return d->usbCableStatus;
}

bool USBGadgetManager::canDetectCableHotplugging() const
{
    Q_D(const USBGadgetManager);
    return d->canDetectCableHotplugging;
}

bool USBGadgetManager::isSystemWideLockActive() const
{
    Q_D(const USBGadgetManager);
    return d->systemWideLockActive;
}

QString USBGadgetManager::systemWideLockOwner() const
{
    Q_D(const USBGadgetManager);
    return d->systemWideLockOwner;
}

QString USBGadgetManager::systemWideLockReason() const
{
    Q_D(const USBGadgetManager);
    return d->systemWideLockReason;
}

Operation* USBGadgetManager::activate(USBGadgetManager::Mode mode, const QVariantMap& arguments)
{
    Q_D(const USBGadgetManager);
    return new DBusVoidOperation(d->interface->Activate(static_cast<uint>(mode), arguments));
}

Operation* USBGadgetManager::deactivate()
{
    Q_D(const USBGadgetManager);
    return new DBusVoidOperation(d->interface->Deactivate());
}

Operation* USBGadgetManager::acquireSystemWideLock(const QString& reason)
{
    Q_D(const USBGadgetManager);
    return new DBusVoidOperation(d->interface->AcquireSystemWideLock(reason));
}

Operation* USBGadgetManager::releaseSystemWideLock()
{
    Q_D(const USBGadgetManager);
    return new DBusVoidOperation(d->interface->ReleaseSystemWideLock());
}

}
