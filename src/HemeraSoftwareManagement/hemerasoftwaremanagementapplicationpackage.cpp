/*
 *
 */

#include "hemerasoftwaremanagementapplicationpackage_p.h"

#include "hemerasoftwaremanagementconstructors_p.h"

#include <QtCore/QJsonArray>

namespace Hemera {

namespace SoftwareManagement {

ApplicationPackage::ApplicationPackage()
    : d(new ApplicationPackageData())
{
}

ApplicationPackage::ApplicationPackage(ApplicationPackageData* data)
    : d(data)
{
}

ApplicationPackage::ApplicationPackage(const ApplicationPackage& other)
    : d(other.d)
{
}

ApplicationPackage::~ApplicationPackage()
{

}

ApplicationPackage& ApplicationPackage::operator=(const ApplicationPackage& rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool ApplicationPackage::operator==(const ApplicationPackage& other) const
{
    return d->applicationId == other.applicationId() && d->version == other.version();
}

bool Hemera::SoftwareManagement::ApplicationPackage::isValid() const
{
    return !d->applicationId.isEmpty();
}

QString ApplicationPackage::applicationId() const
{
    return d->applicationId;
}

QString ApplicationPackage::applicationName() const
{
    return d->applicationName;
}

QString ApplicationPackage::description() const
{
    return d->description;
}

QUrl ApplicationPackage::icon() const
{
    return d->icon;
}

bool ApplicationPackage::isInstalled() const
{
    return d->installed;
}

quint64 ApplicationPackage::installedSize() const
{
    return d->installedSize;
}

QString ApplicationPackage::packageName() const
{
    return d->packageName;
}

quint64 ApplicationPackage::packageSize() const
{
    return d->packageSize;
}

QString ApplicationPackage::version() const
{
    return d->version;
}

ApplicationPackage Constructors::applicationPackageFromData(const QString &applicationId, const QString &applicationName, const QString &description,
                                                            const QUrl &icon, const QString &packageName, const QString &version, quint64 packageSize,
                                                            quint64 installedSize, bool installed)
{
    return ApplicationPackage(new ApplicationPackageData(applicationId, applicationName, description, icon,
                                                         packageName, version, packageSize, installedSize, installed));
}

ApplicationPackage Constructors::applicationPackageFromJson(const QJsonObject& json)
{
    return applicationPackageFromData(json.value(QStringLiteral("applicationId")).toString(), json.value(QStringLiteral("applicationName")).toString(),
                                      json.value(QStringLiteral("description")).toString(), QUrl(json.value(QStringLiteral("icon")).toString()),
                                      json.value(QStringLiteral("packageName")).toString(), json.value(QStringLiteral("version")).toString(),
                                      json.value(QStringLiteral("packageSize")).toInt(), json.value(QStringLiteral("installedSize")).toInt(),
                                      json.value(QStringLiteral("installed")).toBool());
}

QJsonObject Constructors::toJson(const ApplicationPackage &package)
{
    QJsonObject obj;
    obj.insert(QStringLiteral("applicationId"), package.d->applicationId);
    obj.insert(QStringLiteral("applicationName"), package.d->applicationName);
    obj.insert(QStringLiteral("description"), package.d->description);
    obj.insert(QStringLiteral("icon"), package.d->icon.toString());
    obj.insert(QStringLiteral("installed"), package.d->installed);
    obj.insert(QStringLiteral("installedSize"), static_cast<qint64>(package.d->installedSize));
    obj.insert(QStringLiteral("packageName"), package.d->packageName);
    obj.insert(QStringLiteral("packageSize"), static_cast<qint64>(package.d->packageSize));
    obj.insert(QStringLiteral("version"), package.d->version);

    return obj;
}

ApplicationPackages Constructors::applicationPackagesFromJson(const QJsonArray &json)
{
    ApplicationPackages ret;
    ret.reserve(json.size());
    for (const QJsonValue &val : json) {
        ret.append(Constructors::applicationPackageFromJson(val.toObject()));
    }
    return ret;
}

QJsonArray Constructors::toJson(const ApplicationPackages &packages)
{
    QJsonArray ret;
    for (const ApplicationPackage &package : packages) {
        ret.append(Constructors::toJson(package));
    }
    return ret;
}

}

}
