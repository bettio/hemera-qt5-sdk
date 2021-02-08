#include "hemerafingerprints.h"

#include "hemeracommonoperations.h"
#include "hemeraliterals.h"

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>


namespace Hemera
{

Fingerprints::Fingerprints()
    : d(nullptr)
{
}

Fingerprints::~Fingerprints()
{
}

ByteArrayOperation *Fingerprints::globalHardwareId()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::fingerprintsService()),
                                                       Literals::literal(Literals::DBus::fingerprintsPath()),
                                                       Literals::literal(Literals::DBus::fingerprintsInterface()),
                                                       QStringLiteral("GlobalHardwareId"));

    return new DBusByteArrayOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

ByteArrayOperation *Fingerprints::globalSystemId()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::fingerprintsService()),
                                                       Literals::literal(Literals::DBus::fingerprintsPath()),
                                                       Literals::literal(Literals::DBus::fingerprintsInterface()),
                                                       QStringLiteral("GlobalSystemId"));

    return new DBusByteArrayOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

}
