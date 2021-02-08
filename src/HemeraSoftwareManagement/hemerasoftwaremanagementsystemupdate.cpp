#include "hemerasoftwaremanagementsystemupdate.h"

#include "hemerasoftwaremanagementconstructors_p.h"

#include <QtCore/QSharedData>
#include <QtCore/QVersionNumber>

#include <algorithm>

namespace Hemera {

namespace SoftwareManagement {

class SystemUpdateData : public QSharedData
{
public:
    SystemUpdateData() { }
    SystemUpdateData(SystemUpdate::UpdateType updateType, const QString& applianceVersion,
                     quint64 downloadSize, const QString& changelog, const QString &readme)
        : updateType(updateType), applianceVersion(applianceVersion)
        , downloadSize(downloadSize), changelog(changelog), readme(readme) { }
    SystemUpdateData(const SystemUpdateData &other)
        : QSharedData(other), updateType(other.updateType), applianceVersion(other.applianceVersion)
        , downloadSize(other.downloadSize)
        , changelog(other.changelog), readme(other.readme) { }
    ~SystemUpdateData() { }

    SystemUpdate::UpdateType updateType;
    QString applianceVersion;
    quint64 downloadSize;
    QString changelog;
    QString readme;
};

SystemUpdate::SystemUpdate()
    : d(new SystemUpdateData())
{
}

SystemUpdate::SystemUpdate(SystemUpdateData* data)
    : d(data)
{

}

SystemUpdate::SystemUpdate(const SystemUpdate& other)
    : d(other.d)
{
}

SystemUpdate::~SystemUpdate()
{
}

SystemUpdate &SystemUpdate::operator=(const SystemUpdate& rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool SystemUpdate::operator==(const SystemUpdate& other) const
{
    return other.applianceVersion() == applianceVersion();
}

int SystemUpdate::compare(const QString &version) const
{
    return QVersionNumber::compare(QVersionNumber::fromString(applianceVersion()), QVersionNumber::fromString(version));
}

int SystemUpdate::compare(const SystemUpdate &other) const
{
    return compare(other.applianceVersion());
}

QString SystemUpdate::changelog() const
{
    return d->changelog;
}

QString SystemUpdate::readme() const
{
    return d->readme;
}

quint64 SystemUpdate::downloadSize() const
{
    return d->downloadSize;
}

SystemUpdate::UpdateType SystemUpdate::updateType() const
{
    return d->updateType;
}

bool SystemUpdate::isValid() const
{
    return d->updateType != UpdateType::Unknown && !d->applianceVersion.isEmpty();
}

QString SystemUpdate::applianceVersion() const
{
    return d->applianceVersion;
}

SystemUpdate Constructors::systemUpdateFromJson(const QJsonObject& json)
{
    // Check if valid
    if (!json.contains(QStringLiteral("version"))) {
        return SystemUpdate();
    }

    // Find out which update type this is
    SystemUpdate::UpdateType type = SystemUpdate::UpdateType::Unknown;
    QString typeAsString = json.value(QStringLiteral("artifact_type")).toString();
    if (typeAsString == QStringLiteral("recovery")) {
        type = SystemUpdate::UpdateType::RecoveryUpdate;
    } else if (typeAsString == QStringLiteral("update")) {
        type = SystemUpdate::UpdateType::IncrementalUpdate;
    }

    SystemUpdateData *data = new SystemUpdateData(type, json.value(QStringLiteral("version")).toString(),
                                                  json.value(QStringLiteral("download_size")).toInt(),
                                                  json.value(QStringLiteral("changelog")).toString(),
                                                  json.value(QStringLiteral("readme")).toString());

    return SystemUpdate(data);
}

QJsonObject Constructors::toJson(const SystemUpdate &update)
{
    QJsonObject obj;
    switch (update.d->updateType) {
        case SystemUpdate::UpdateType::RecoveryUpdate:
            obj.insert(QStringLiteral("artifact_type"), QStringLiteral("recovery"));
            break;
        case SystemUpdate::UpdateType::IncrementalUpdate:
            obj.insert(QStringLiteral("artifact_type"), QStringLiteral("update"));
            break;
        default:
            break;
    }
    obj.insert(QStringLiteral("changelog"), update.d->changelog);
    obj.insert(QStringLiteral("readme"), update.d->readme);
    obj.insert(QStringLiteral("download_size"), static_cast<qint64>(update.d->downloadSize));
    obj.insert(QStringLiteral("version"), update.d->applianceVersion);

    return obj;
}

}

}

#include "moc_hemerasoftwaremanagementsystemupdate.cpp"
