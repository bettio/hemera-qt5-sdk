#ifndef HEMERA_HEMERAAPPLIANCECRYPTO_H
#define HEMERA_HEMERAAPPLIANCECRYPTO_H

#include <QtCore/QObject>
#include <HemeraCore/Global>

namespace Hemera {

class ByteArrayOperation;
class SSLCertificateOperation;

class HEMERA_QT5_SDK_EXPORT ApplianceCrypto : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ApplianceCrypto)

public:
    static ByteArrayOperation *deviceKey();
    static SSLCertificateOperation *clientSSLCertificate();

    static ByteArrayOperation *localCA();
    static ByteArrayOperation *signSSLCertificate(const QByteArray &certificateSigningRequest);

private:
    ApplianceCrypto();
    virtual ~ApplianceCrypto();
    class Private;
    Private * const d;
};

}

#endif // HEMERA_HEMERAAPPLIANCECRYPTO_H
