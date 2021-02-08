#ifndef HEMERA_SOFTWAREMANAGEMENT_APPLICATIONPACKAGE_P_H
#define HEMERA_SOFTWAREMANAGEMENT_APPLICATIONPACKAGE_P_H

#include "hemerasoftwaremanagementapplicationpackage.h"

#include <QtCore/QSharedData>
#include <QtCore/QUrl>

namespace Hemera {

namespace SoftwareManagement {

class ApplicationPackageData : public QSharedData
{
public:
    ApplicationPackageData() : packageSize(0), installedSize(0), installed(false) { }
    ApplicationPackageData(const QString &applicationId, const QString &applicationName, const QString &description, const QUrl &icon,
                           const QString &packageName, const QString &version, quint64 packageSize,
                           quint64 installedSize, bool installed)
        : applicationId(applicationId), applicationName(applicationName), description(description), icon(icon), packageName(packageName)
        , version(version), packageSize(packageSize), installedSize(installedSize), installed(installed) { }
    ApplicationPackageData(const ApplicationPackageData &other)
        : QSharedData(other), applicationId(other.applicationId), applicationName(other.applicationName), description(other.description), icon(other.icon)
        , packageName(other.packageName), version(other.version), packageSize(other.packageSize)
        , installedSize(other.installedSize), installed(other.installed) { }
    ~ApplicationPackageData() { }

    QString applicationId;
    QString applicationName;
    QString description;
    QUrl icon;
    QString packageName;
    QString version;
    quint64 packageSize;
    quint64 installedSize;
    bool installed;
};

}

}

#endif
