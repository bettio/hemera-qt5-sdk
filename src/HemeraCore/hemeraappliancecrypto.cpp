#include "hemeraappliancecrypto.h"

#include "hemeracommonoperations.h"
#include "hemeraliterals.h"

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>


namespace Hemera
{

ApplianceCrypto::ApplianceCrypto()
    : d(nullptr)
{
}

ApplianceCrypto::~ApplianceCrypto()
{
}

ByteArrayOperation *ApplianceCrypto::deviceKey()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::fingerprintsService()),
                                                       Literals::literal(Literals::DBus::applianceCryptoPath()),
                                                       Literals::literal(Literals::DBus::applianceCryptoInterface()),
                                                       QStringLiteral("DeviceKey"));

    return new DBusByteArrayOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

SSLCertificateOperation *ApplianceCrypto::clientSSLCertificate()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::fingerprintsService()),
                                                       Literals::literal(Literals::DBus::applianceCryptoPath()),
                                                       Literals::literal(Literals::DBus::applianceCryptoInterface()),
                                                       QStringLiteral("ClientSSLCertificate"));

    return new DBusSSLCertificateOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

ByteArrayOperation *ApplianceCrypto::localCA()
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::fingerprintsService()),
                                                       Literals::literal(Literals::DBus::applianceCryptoPath()),
                                                       Literals::literal(Literals::DBus::applianceCryptoInterface()),
                                                       QStringLiteral("LocalCA"));

    return new DBusByteArrayOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

ByteArrayOperation *ApplianceCrypto::signSSLCertificate(const QByteArray &certificateSigningRequest)
{
    QDBusMessage dmsg = QDBusMessage::createMethodCall(Literals::literal(Literals::DBus::fingerprintsService()),
                                                       Literals::literal(Literals::DBus::applianceCryptoPath()),
                                                       Literals::literal(Literals::DBus::applianceCryptoInterface()),
                                                       QStringLiteral("SignSSLCertificate"));
    dmsg.setArguments(QList<QVariant>() << certificateSigningRequest);

    return new DBusByteArrayOperation(QDBusConnection::systemBus().asyncCall(dmsg, -1));
}

}
