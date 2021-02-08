#ifndef HEMERA_HEMERAORBITHANDLER_P_H
#define HEMERA_HEMERAORBITHANDLER_P_H

#include <HemeraCore/Gravity>

#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

class ComIspirataHemeraParsecInterface;
class ComIspirataHemeraDBusObjectInterface;

namespace Hemera
{

class DBusUIntOperation;

class Gravity::Private
{
public:
    Private() {}

    ComIspirataHemeraParsecInterface *parsec;
    ComIspirataHemeraDBusObjectInterface *parsecDBusObject;

    // Orbit Control
    QString activeOrbit;
    QVariantMap inhibitionReasons;
    bool isInhibited;
    QStringList availableUpdates;
    bool deviceInDeveloperMode;

    // Orbit Manager
    QString name;
    QString baseConfigPath;

    Hemera::Operation *requestOrbitSwitchInhibition(const QString& reason);
    Hemera::Operation *releaseOrbitSwitchInhibition();
};

}

#endif
