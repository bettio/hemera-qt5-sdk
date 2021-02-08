#include "hemeraremovablestorage.h"

#include "hemeraasyncinitobject_p.h"

#include "hemeracommonoperations.h"
#include "hemeraliterals.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

#include "removablestoragemanagerinterface.h"

Q_LOGGING_CATEGORY(LOG_REMOVABLESTORAGE, "Hemera::RemovableStorage")

namespace Hemera
{

class RemovableStorage::DeviceData : public QSharedData
{
public:
    DeviceData() : mounted(false), type(RemovableStorage::Device::Type::Unknown), size(0) { }
    DeviceData(const RemovableStorage::DeviceData &other)
        : QSharedData(other), mounted(other.mounted), mountPoint(other.mountPoint), path(other.path)
        , type(other.type), filesystem(other.filesystem), size(other.size) { }
    ~DeviceData() { }

    bool mounted;
    QString mountPoint;
    QString path;
    RemovableStorage::Device::Type type;
    QString filesystem;
    qint64 size;
};

RemovableStorage::Device::Device()
    : d(new RemovableStorage::DeviceData())
{
}

RemovableStorage::Device::Device(const RemovableStorage::Device& other)
    : d(other.d)
{
}

RemovableStorage::Device::Device(const QJsonObject& data)
    : d(new RemovableStorage::DeviceData())
{
    // Parse the JSON object and populate the private class
    d->mounted = data.value(QStringLiteral("mounted")).toBool();
    d->mountPoint = data.value(QStringLiteral("mountPoint")).toString();
    d->path = data.value(QStringLiteral("path")).toString();
    d->type = static_cast< Device::Type >(data.value(QStringLiteral("type")).toInt());
    d->filesystem = data.value(QStringLiteral("filesystem")).toString();
    d->size = data.value(QStringLiteral("size")).toInt();
}

RemovableStorage::Device::~Device()
{
}

RemovableStorage::Device& RemovableStorage::Device::operator=(const RemovableStorage::Device& rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool RemovableStorage::Device::operator==(const RemovableStorage::Device& other) const
{
    return d->path == other.path() && d->mounted == other.mounted() && d->mountPoint == other.mountPoint();
}

bool RemovableStorage::Device::mounted() const
{
    return d->mounted;
}

QString RemovableStorage::Device::mountPoint() const
{
    return d->mountPoint;
}

QString RemovableStorage::Device::path() const
{
    return d->path;
}

RemovableStorage::Device::Type RemovableStorage::Device::type() const
{
    return d->type;
}

QString RemovableStorage::Device::filesystem() const
{
    return d->filesystem;
}

qint64 RemovableStorage::Device::size() const
{
    return d->size;
}


class RemovableStoragePrivate : public AsyncInitObjectPrivate
{
public:
    RemovableStoragePrivate(RemovableStorage *q) : AsyncInitObjectPrivate(q) {}

    QList< RemovableStorage::Device > devices;
    QHash< QString, RemovableStorage::Device > mountPointToDevice;
    com::ispirata::Hemera::RemovableStorageManager *removableStorageManagerInterface;

    void loadDevicesFromJson(const QByteArray &data);
    RemovableStorage::Device deviceFromJsonDocument(const QByteArray &data);
};

void RemovableStoragePrivate::loadDevicesFromJson(const QByteArray &data)
{
    devices.clear();
    mountPointToDevice.clear();
    QJsonDocument deviceList = QJsonDocument::fromJson(data);
    if (!deviceList.isArray()) {
        qCWarning(LOG_REMOVABLESTORAGE) << "Devices changed, but could not parse returned list!" << data;
        return;
    }

    for (const QJsonValue &device : deviceList.array()) {
        RemovableStorage::Device newDevice(device.toObject());
        if (newDevice.mounted() && !newDevice.mountPoint().isEmpty()) {
           mountPointToDevice.insert(newDevice.mountPoint(), newDevice);
        }
        devices.append(newDevice);
    }
}

RemovableStorage::Device RemovableStoragePrivate::deviceFromJsonDocument(const QByteArray &data)
{
    QJsonDocument deviceDoc = QJsonDocument::fromJson(data);
    if (!deviceDoc.isObject()) {
        qCWarning(LOG_REMOVABLESTORAGE) << "Device received, but could not parse device data!" << data;
        return RemovableStorage::Device();
    }
    return RemovableStorage::Device(deviceDoc.object());
}

RemovableStorage::RemovableStorage(QObject *parent)
    : AsyncInitObject(*new RemovableStoragePrivate(this), parent)
{
}

RemovableStorage::~RemovableStorage()
{
}

void RemovableStorage::initImpl()
{
    Q_D(RemovableStorage);

    // Create connection to the Application Handler
    d->removableStorageManagerInterface = new com::ispirata::Hemera::RemovableStorageManager(Literals::literal(Literals::DBus::gravityCenterService()),
                                                                                             Literals::literal(Literals::DBus::removableStorageManagerPath()),
                                                                                             QDBusConnection::systemBus(), this);

    if (!d->removableStorageManagerInterface->isValid()) {
        setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                     tr("The remote DBus Manager is not available. The daemon is probably not running."));
        return;
    }

    // Monitor changes to devices
    connect(d->removableStorageManagerInterface, &com::ispirata::Hemera::RemovableStorageManager::DevicesChanged, this, [this, d] (const QByteArray &data) {
        d->loadDevicesFromJson(data);
        Q_EMIT devicesChanged();
    });

    connect(d->removableStorageManagerInterface, &com::ispirata::Hemera::RemovableStorageManager::DeviceAdded, this, [this, d] (const QByteArray &data) {
        RemovableStorage::Device addedDevice = d->deviceFromJsonDocument(data);
        if (addedDevice.isValid()) {
            Q_EMIT deviceAdded(addedDevice);
        }
    });

    connect(d->removableStorageManagerInterface, &com::ispirata::Hemera::RemovableStorageManager::DeviceRemoved, this, &Hemera::RemovableStorage::deviceRemoved);

    connect(d->removableStorageManagerInterface, &com::ispirata::Hemera::RemovableStorageManager::DeviceMounted, this, [this, d] (const QByteArray &data) {
        RemovableStorage::Device mountedDevice = d->deviceFromJsonDocument(data);
        if (mountedDevice.isValid()) {
            Q_EMIT deviceMounted(mountedDevice);
        }
    });

    connect(d->removableStorageManagerInterface, &com::ispirata::Hemera::RemovableStorageManager::DeviceUnmounted, this, [this, d] (const QByteArray &data) {
        RemovableStorage::Device unmountedDevice = d->deviceFromJsonDocument(data);
        if (unmountedDevice.isValid()) {
            Q_EMIT deviceUnmounted(unmountedDevice);
        }
    });

    // Cache the list
    QDBusPendingReply< QByteArray > pendingDevices =  d->removableStorageManagerInterface->ListDevices();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingDevices, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, d, watcher] {
        QDBusPendingReply< QByteArray > reply = *watcher;
        if (reply.isError()) {
            qCWarning(LOG_REMOVABLESTORAGE)  << "Error retrieving devices data! Setting ready anyway...";
        } else {
            d->loadDevicesFromJson(reply.value());
        }

        watcher->deleteLater();
        setReady();
    });
}

QList< RemovableStorage::Device > RemovableStorage::devices() const
{
    Q_D(const RemovableStorage);
    return d->devices;
}

QList< RemovableStorage::Device > RemovableStorage::mountedDevices() const
{
    Q_D(const RemovableStorage);

    QList< RemovableStorage::Device > ret;
    for (const RemovableStorage::Device &device : d->devices) {
        if (device.mounted()) {
            ret.append(device);
        }
    }

    return ret;
}

QList< RemovableStorage::Device > RemovableStorage::unmountedDevices() const
{
    Q_D(const RemovableStorage);

    QList< RemovableStorage::Device > ret;
    for (const RemovableStorage::Device &device : d->devices) {
        if (!device.mounted()) {
            ret.append(device);
        }
    }

    return ret;
}

StringOperation* RemovableStorage::mount(const Device& device, MountOptions options)
{
    Q_D(RemovableStorage);
    return new DBusStringOperation(d->removableStorageManagerInterface->Mount(device.path(), static_cast<int>(options)));
}

Operation *RemovableStorage::unmount(const RemovableStorage::Device &device)
{
    Q_D(RemovableStorage);
    return new DBusVoidOperation(d->removableStorageManagerInterface->Unmount(device.path()));
}

Operation *RemovableStorage::unmount(const QString &mountPoint)
{
    Q_D(RemovableStorage);
    if (!d->mountPointToDevice.contains(mountPoint)) {
        return new FailureOperation(Hemera::RemovableStorage::Errors::notMounted(),
                                    QStringLiteral("Nothing mounted on mount point %1").arg(mountPoint));
    }
    return unmount(d->mountPointToDevice.value(mountPoint));
}

}
