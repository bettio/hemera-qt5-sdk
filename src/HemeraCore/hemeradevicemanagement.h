#ifndef HEMERA_HEMERADEVICEMANAGEMENT_H
#define HEMERA_HEMERADEVICEMANAGEMENT_H

#include <QtCore/QObject>
#include <HemeraCore/Global>

class QDateTime;
class QLocale;
class QTimeZone;

namespace Hemera {

class Operation;

class HEMERA_QT5_SDK_EXPORT DeviceManagement : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DeviceManagement)

public:
    Q_INVOKABLE static Operation *shutdown();
    Q_INVOKABLE static Operation *reboot();
    Q_INVOKABLE static Operation *factoryReset();

    Q_INVOKABLE static Operation *setGlobalLocale(const QLocale &locale);
    Q_INVOKABLE static Operation *setGlobalTimeZone(const QTimeZone &timeZone);

    Q_INVOKABLE static Operation *setSystemDateTime(const QDateTime &dateTime);

private:
    DeviceManagement();
    virtual ~DeviceManagement();
    class Private;
    Private * const d;
};

}

#endif // HEMERA_HEMERADEVICEMANAGEMENT_H
