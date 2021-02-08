/*
 *
 */

#include "hemeracxxsoftwaremanagementrepository.h"

#include <HemeraSoftwareManagement/Repository>

#include <QtCore/QSharedData>
#include <QtCore/QUrl>
#include <QJsonArray>

namespace HemeraCxx {

namespace SoftwareManagement {

class RepositoryData : public QSharedData
{
public:
    RepositoryData() { }
    RepositoryData(const RepositoryData &other)
        : QSharedData(other), proxy(other.proxy) { }
    RepositoryData(const Hemera::SoftwareManagement::Repository &data)
        : proxy(data) {}
    ~RepositoryData() { }

    Hemera::SoftwareManagement::Repository proxy;
};

Repository::Repository()
{
}

Repository::Repository(RepositoryData* data)
    : d(data)
{
}

Repository::Repository(const Hemera::SoftwareManagement::Repository& other)
    : d(new RepositoryData(other))
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
    return d->proxy.isValid();
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
    return other.d->proxy == d->proxy;
}

std::string Repository::name() const
{
    return d->proxy.name().toStdString();
}

std::vector< std::string > Repository::urls() const
{
    return qStringListToVector(d->proxy.urls());
}

}

}
