/*
 *
 */

#include "hemerasoftwaremanagementrepository.h"

#include "hemerasoftwaremanagementconstructors_p.h"

#include <QtCore/QSharedData>
#include <QtCore/QUrl>
#include <QJsonArray>

namespace Hemera {

namespace SoftwareManagement {

class RepositoryData : public QSharedData
{
public:
    RepositoryData() : valid(false) {}
    RepositoryData(const QString &name, const QStringList &urls)
        : valid(true), name(name), urls(urls) {}
    RepositoryData(const RepositoryData &other)
        : QSharedData(other), valid(other.valid), name(other.name), urls(other.urls) {}
    ~RepositoryData() { }

    bool valid;

    QString name;
    QStringList urls;
};

Repository::Repository()
{
}

Repository::Repository(RepositoryData* data)
    : d(data)
{
}

Repository::Repository(const Repository &other)
    : d(other.d)
{

}

Repository::~Repository()
{

}

bool Repository::isValid() const
{
    return d->valid;
}

Repository &Repository::operator=(const Repository &rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool Repository::operator==(const Repository &other) const
{
    return other.urls() == d->urls;
}

QString Repository::name() const
{
    return d->name;
}

QStringList Repository::urls() const
{
    return d->urls;
}

Repository Constructors::repositoryFromData(const QString &name, const QStringList &urls)
{
    return Repository(new RepositoryData(name, urls));
}

Repository Constructors::repositoryFromJson(const QJsonObject& json)
{
    QStringList urls = [] (const QJsonArray &vurls) { QStringList ret; for (const QJsonValue &url : vurls) ret.append(url.toString()); return ret; }
                       (json.value(QStringLiteral("urls")).toArray());
    return repositoryFromData(json.value(QStringLiteral("name")).toString(), urls);
}

QJsonObject Constructors::toJson(const Repository &repository)
{
    QJsonObject obj;
    obj.insert(QStringLiteral("name"), repository.d->name);
    obj.insert(QStringLiteral("urls"), QJsonArray::fromStringList(repository.d->urls));

    return obj;
}

Repositories Constructors::repositoriesFromJson(const QJsonArray &json)
{
    Repositories ret;
    ret.reserve(json.size());
    for (const QJsonValue &val : json) {
        ret.append(repositoryFromJson(val.toObject()));
    }
    return ret;
}

QJsonArray Constructors::toJson(const Repositories &repositories)
{
    QJsonArray ret;
    for (const Repository &repository : repositories) {
        ret.append(toJson(repository));
    }
    return ret;
}

}

}
