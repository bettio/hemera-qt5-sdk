#ifndef HEMERA_HEMERAREMOVABLESTORAGE_H
#define HEMERA_HEMERAREMOVABLESTORAGE_H

#include <HemeraCore/AsyncInitObject>

#include <QtCore/QSharedDataPointer>

class QDateTime;
class QLocale;
class QTimeZone;

namespace Hemera {

class Operation;
class StringOperation;

class RemovableStoragePrivate;
class HEMERA_QT5_SDK_EXPORT RemovableStorage : public AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RemovableStorage)
    Q_DECLARE_PRIVATE_D(d_h_ptr, RemovableStorage)

public:
    class Errors {
    public:
        static constexpr QLatin1String alreadyMounted() { return QLatin1String("com.ispirata.Hemera.Error.RemovableStorage.AlreadyMounted"); }
        static constexpr QLatin1String noSuchDevice() { return QLatin1String("com.ispirata.Hemera.Error.RemovableStorage.NoSuchDevice"); }
        static constexpr QLatin1String notMounted() { return QLatin1String("com.ispirata.Hemera.Error.RemovableStorage.NotMounted"); }
        static constexpr QLatin1String notWriteable() { return QLatin1String("com.ispirata.Hemera.Error.RemovableStorage.NotWriteable"); }
        static constexpr QLatin1String notOwnedByApplication() { return QLatin1String("com.ispirata.Hemera.Error.RemovableStorage.NotOwnedByApplication"); }
        static constexpr QLatin1String unsupportedFilesystem() { return QLatin1String("com.ispirata.Hemera.Error.RemovableStorage.UnsupportedFS"); }
    };

    enum MountOption {
        NoOption = 0,
        ReadOnly = 1 << 0
    };
    Q_ENUM(MountOption)
    Q_DECLARE_FLAGS(MountOptions, MountOption)
    Q_FLAGS(MountOptions)

    class DeviceData;
    class Device {
    public:
        enum class Type {
            Unknown,
            USB,
            SDCard
        };

        Device();
        /// Copy constructor
        Device(const Device &other);
        ~Device();

        Device &operator=(const Device &rhs);
        bool operator==(const Device &other) const;
        inline bool operator!=(const Device &other) const { return !operator==(other); }

        inline bool isValid() const { return !path().isEmpty(); }

        bool mounted() const;
        QString mountPoint() const;
        QString path() const;
        Type type() const;
        QString filesystem() const;
        qint64 size() const;

    private:
        Device(const QJsonObject& data);

        QSharedDataPointer<DeviceData> d;

        friend class RemovableStorage;
        friend class RemovableStoragePrivate;
    };

    explicit RemovableStorage(QObject *parent = nullptr);
    virtual ~RemovableStorage();

    QList< Device > devices() const;
    QList< Device > mountedDevices() const;
    QList< Device > unmountedDevices() const;
    StringOperation *mount(const Device &device = Device(), MountOptions options = NoOption);
    Operation *unmount(const Device &device);
    Operation *unmount(const QString &mountPoint);

Q_SIGNALS:
    void devicesChanged();
    void deviceAdded(const RemovableStorage::Device &device);
    void deviceRemoved(const QString &deviceName);
    void deviceMounted(const RemovableStorage::Device &device);
    void deviceUnmounted(const RemovableStorage::Device &device);

protected:
    virtual void initImpl() override final;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Hemera::RemovableStorage::MountOptions)

#endif // HEMERA_HEMERAREMOVABLESTORAGE_H
