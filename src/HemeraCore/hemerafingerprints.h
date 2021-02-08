#ifndef HEMERA_HEMERAFINGERPRINTS_H
#define HEMERA_HEMERAFINGERPRINTS_H

#include <QtCore/QObject>
#include <HemeraCore/Global>

namespace Hemera {

class ByteArrayOperation;

class HEMERA_QT5_SDK_EXPORT Fingerprints : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Fingerprints)

public:
    static ByteArrayOperation *globalHardwareId();
    static ByteArrayOperation *globalSystemId();

private:
    Fingerprints();
    virtual ~Fingerprints();
    class Private;
    Private * const d;
};

}

#endif // HEMERA_HEMERAFINGERPRINTS_H
