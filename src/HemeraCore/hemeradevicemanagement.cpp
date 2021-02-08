#include "hemeradevicemanagement.h"

#include "hemeracommonoperations.h"
#include "hemeraliterals.h"

#include <QtCore/QDateTime>
#include <QtCore/QLocale>
#include <QtCore/QTimeZone>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>


namespace Hemera
{

DeviceManagement::DeviceManagement()
    : d(nullptr)
{
}

DeviceManagement::~DeviceManagement()
{
}

Operation *DeviceManagement::reboot()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::gravityCenterService()),
                                                       Literals::literal(Literals::DBus::deviceManagementPath()),
                                                       Literals::literal(Literals::DBus::deviceManagementInterface()),
                                                       QStringLiteral("Reboot"));

    return new DBusVoidOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

Operation *DeviceManagement::shutdown()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::gravityCenterService()),
                                                       Literals::literal(Literals::DBus::deviceManagementPath()),
                                                       Literals::literal(Literals::DBus::deviceManagementInterface()),
                                                       QStringLiteral("Shutdown"));

    return new DBusVoidOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

Operation *DeviceManagement::factoryReset()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::gravityCenterService()),
                                                       Literals::literal(Literals::DBus::deviceManagementPath()),
                                                       Literals::literal(Literals::DBus::deviceManagementInterface()),
                                                       QStringLiteral("FactoryReset"));

    return new DBusVoidOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

Operation *DeviceManagement::setGlobalLocale(const QLocale &locale)
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::gravityCenterService()),
                                                       Literals::literal(Literals::DBus::deviceManagementPath()),
                                                       Literals::literal(Literals::DBus::deviceManagementInterface()),
                                                       QStringLiteral("SetGlobalLocale"));
    dmsg.setArguments(QList<QVariant>() << locale.name());

    return new DBusVoidOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

Operation *DeviceManagement::setGlobalTimeZone(const QTimeZone &timeZone)
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::gravityCenterService()),
                                                       Literals::literal(Literals::DBus::deviceManagementPath()),
                                                       Literals::literal(Literals::DBus::deviceManagementInterface()),
                                                       QStringLiteral("SetGlobalTimeZone"));
    dmsg.setArguments(QList<QVariant>() << QLatin1String(timeZone.id()));

    return new DBusVoidOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

Operation *DeviceManagement::setSystemDateTime(const QDateTime &dateTime)
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::gravityCenterService()),
                                                       Literals::literal(Literals::DBus::deviceManagementPath()),
                                                       Literals::literal(Literals::DBus::deviceManagementInterface()),
                                                       QStringLiteral("SetSystemDateTime"));
    dmsg.setArguments(QList<QVariant>() << dateTime.toMSecsSinceEpoch());

    return new DBusVoidOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

}
