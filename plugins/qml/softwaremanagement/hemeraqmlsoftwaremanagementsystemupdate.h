#ifndef HEMERA_QMLSYSTEMUPDATE_H
#define HEMERA_QMLSYSTEMUPDATE_H

#include <HemeraCore/Global>

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

#include <HemeraSoftwareManagement/SystemUpdate>

namespace Hemera {

namespace Qml {

namespace SoftwareManagement {

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
class SystemUpdate : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool valid READ isValid NOTIFY systemUpdateChanged)
    Q_PROPERTY(QString applianceVersion READ applianceVersion NOTIFY systemUpdateChanged)
    Q_PROPERTY(QString hemeraVersion READ hemeraVersion NOTIFY systemUpdateChanged)
    Q_PROPERTY(quint64 downloadSize READ downloadSize NOTIFY systemUpdateChanged)
    Q_PROPERTY(QString changelog READ changelog NOTIFY systemUpdateChanged)
    Q_PROPERTY(QString readme READ readme NOTIFY systemUpdateChanged)
    Q_PROPERTY(Hemera::SoftwareManagement::SystemUpdate::UpdateType updateType READ updateType NOTIFY systemUpdateChanged)

public:
    /// Creates a system update
    SystemUpdate(QObject *parent);
    virtual ~SystemUpdate();

    /// @returns Whether this SystemUpdate is valid
    bool isValid() const;

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
    Hemera::SoftwareManagement::SystemUpdate::UpdateType updateType() const;

Q_SIGNALS:
    void systemUpdateChanged();

private:
    void setSystemUpdate(const Hemera::SoftwareManagement::SystemUpdate &systemUpdate);

    class Private;
    Private * const d;

    friend class ApplianceManager;
};

}

}

}

#endif // HEMERA_QMLSYSTEMUPDATE_H
