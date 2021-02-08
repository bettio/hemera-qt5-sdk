#include "hemerasoftwaremanagementapplicationupdate.h"

#include "hemerasoftwaremanagementconstructors_p.h"

#include <QtCore/QJsonArray>
#include <QtCore/QSharedData>

namespace Hemera {

namespace SoftwareManagement {

class ApplicationUpdateData : public QSharedData
{
public:
    ApplicationUpdateData() : downloadSize(0), installedSize(0) { }
    ApplicationUpdateData(const QString& applicationId, const QString& applicationName, const QString& description,
                          const QString& installedVersion, const QString& version, quint64 downloadSize, qint32 installedSize, const QString& changelog)
        : applicationId(applicationId), applicationName(applicationName), description(description), installedVersion(installedVersion)
        , version(version), downloadSize(downloadSize), installedSize(installedSize), changelog(changelog) { }
    ApplicationUpdateData(const ApplicationUpdateData &other)
        : QSharedData(other), applicationId(other.applicationId), applicationName(other.applicationName), description(other.description)
        , installedVersion(other.installedVersion), version(other.version), downloadSize(other.downloadSize)
        , installedSize(other.installedSize), changelog(other.changelog) { }
    ~ApplicationUpdateData() { }

    QString applicationId;
    QString applicationName;
    QString description;
    QString installedVersion;
    QString version;
    quint64 downloadSize;
    qint32 installedSize;
    QString changelog;
};

ApplicationUpdate::ApplicationUpdate()
    :d(new ApplicationUpdateData())
{

}

ApplicationUpdate::ApplicationUpdate(ApplicationUpdateData* data)
    : d(data)
{
}

ApplicationUpdate::ApplicationUpdate(const ApplicationUpdate& other)
    : d(other.d)
{
}

ApplicationUpdate::~ApplicationUpdate()
{

}

bool ApplicationUpdate::isValid() const
{
    return !d->applicationId.isEmpty() && !d->version.isEmpty();
}


ApplicationUpdate& ApplicationUpdate::operator=(const ApplicationUpdate& rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool ApplicationUpdate::operator==(const ApplicationUpdate& other) const
{
    return d->applicationId == other.applicationId() && d->installedVersion == other.installedVersion() && d->version == other.version();
}

QString ApplicationUpdate::applicationId() const
{
    return d->applicationId;
}

QString ApplicationUpdate::applicationName() const
{
    return d->applicationName;
}

QString ApplicationUpdate::changelog() const
{
    return d->changelog;
}

QString ApplicationUpdate::description() const
{
    return d->description;
}

quint64 ApplicationUpdate::downloadSize() const
{
    return d->downloadSize;
}

qint32 ApplicationUpdate::installedSize() const
{
    return d->installedSize;
}

QString ApplicationUpdate::installedVersion() const
{
    return d->installedVersion;
}

QString ApplicationUpdate::version() const
{
    return d->version;
}

ApplicationUpdate Constructors::applicationUpdateFromData(const QString &applicationId, const QString &applicationName, const QString &description,
                                                          const QString &installedVersion, const QString &version, quint64 downloadSize,
                                                          qint32 installedSize, const QString &changelog)
{
    return ApplicationUpdate(new ApplicationUpdateData(applicationId, applicationName, description,
                                                       installedVersion, version, downloadSize, installedSize, changelog));
}

ApplicationUpdate Constructors::applicationUpdateFromJson(const QJsonObject& json)
{
    return applicationUpdateFromData(json.value(QStringLiteral("applicationId")).toString(), json.value(QStringLiteral("applicationName")).toString(),
                                     json.value(QStringLiteral("description")).toString(), json.value(QStringLiteral("installedVersion")).toString(),
                                     json.value(QStringLiteral("version")).toString(), json.value(QStringLiteral("downloadSize")).toInt(),
                                     json.value(QStringLiteral("installedSize")).toInt(), json.value(QStringLiteral("changelog")).toString());
}

QJsonObject Constructors::toJson(const ApplicationUpdate &update)
{
    QJsonObject obj;
    obj.insert(QStringLiteral("applicationId"), update.d->applicationId);
    obj.insert(QStringLiteral("applicationName"), update.d->applicationName);
    obj.insert(QStringLiteral("changelog"), update.d->changelog);
    obj.insert(QStringLiteral("description"), update.d->description);
    obj.insert(QStringLiteral("downloadSize"), static_cast<qint64>(update.d->downloadSize));
    obj.insert(QStringLiteral("installedSize"), update.d->installedSize);
    obj.insert(QStringLiteral("installedVersion"), update.d->installedVersion);
    obj.insert(QStringLiteral("version"), update.d->version);

    return obj;
}

ApplicationUpdates Constructors::applicationUpdatesFromJson(const QJsonArray &json)
{
    ApplicationUpdates ret;
    ret.reserve(json.size());
    for (const QJsonValue &val : json) {
        ret.append(applicationUpdateFromJson(val.toObject()));
    }
    return ret;
}

QJsonArray Constructors::toJson(const ApplicationUpdates &updates)
{
    QJsonArray ret;
    for (const ApplicationUpdate &update : updates) {
        ret.append(toJson(update));
    }
    return ret;
}

}

}
