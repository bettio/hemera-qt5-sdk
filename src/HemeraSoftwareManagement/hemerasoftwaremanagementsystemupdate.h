#ifndef HEMERA_SYSTEMUPDATE_H
#define HEMERA_SYSTEMUPDATE_H

#include <HemeraCore/Global>

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

namespace Hemera {

namespace SoftwareManagement {

class SystemUpdateData;
/**
 * @class SystemUpdate
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagementsystemupdate.h <HemeraSoftwareManagement/SystemUpdate>
 *
 * @brief Represents an available, and uninstalled, system update
 *
 * SystemUpdate represents a system update known to the SoftwareManager. This class is meant
 * to review the update and to submit the update to the SoftwareManager in case it should
 * be installed. For every Hemera system, it might exist one and only one System Update at a time.
 *
 * @note SystemUpdate is an implicitly shared object.
 *
 * @sa Hemera::SoftwareManagement::SoftwareManager
 */
class HEMERA_QT5_SDK_EXPORT SystemUpdate {
    Q_GADGET
public:
    enum class UpdateType : quint16 {
        Unknown = 0,
        IncrementalUpdate = 1,
        RecoveryUpdate = 2
    };
    Q_ENUM(UpdateType)

    /// Creates an invalid system update
    SystemUpdate();
    SystemUpdate(const SystemUpdate &other);
    ~SystemUpdate();

    /// @returns Whether this SystemUpdate is valid
    bool isValid() const;

    SystemUpdate &operator=(const SystemUpdate &rhs);
    bool operator==(const SystemUpdate &other) const;
    inline bool operator!=(const SystemUpdate &other) const { return !operator==(other); }

    inline bool operator==(const QString &version) const { return compare(version) == 0; }
    inline bool operator<(const QString &version) const { return compare(version) < 0; }
    inline bool operator<=(const QString &version) const { return compare(version) <= 0; }
    inline bool operator>(const QString &version) const { return compare(version) > 0; }
    inline bool operator>=(const QString &version) const { return compare(version) >= 0; }

    inline bool operator<(const SystemUpdate &other) const { return compare(other) < 0; }
    inline bool operator<=(const SystemUpdate &other) const { return compare(other) <= 0; }
    inline bool operator>(const SystemUpdate &other) const { return compare(other) > 0; }
    inline bool operator>=(const SystemUpdate &other) const { return compare(other) >= 0; }

    /// @returns The Hemera version the system will be updated to
    QString applianceVersion() const;
    /// @returns The Hemera version the system will be updated to
    QString hemeraVersion() const;
    /// @returns The download size, in bytes, of this update
    quint64 downloadSize() const;;
    /// @returns The changelog between updates of the updateable application
    QString changelog() const;
    /// @returns A readme message to be accepted by the user before the update, if any
    QString readme() const;
    /// @returns Which kind of update this is
    UpdateType updateType() const;

    int compare(const QString &version) const;
    int compare(const SystemUpdate &other) const;

private:
    QSharedDataPointer<SystemUpdateData> d;

    explicit SystemUpdate(SystemUpdateData *data);

    friend class Constructors;
};

}

}

#endif // HEMERA_SYSTEMUPDATE_H
