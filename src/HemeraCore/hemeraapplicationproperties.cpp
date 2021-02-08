#include "hemeraapplicationproperties.h"

namespace Hemera
{

class ApplicationProperties::Private
{
public:
    QString id;

    QString name;
    QString version;
    QString organization;
};

ApplicationProperties::ApplicationProperties()
    : d(new Private)
{
}

ApplicationProperties::~ApplicationProperties()
{
    delete d;
}

QString ApplicationProperties::id() const
{
    return d->id;
}

void ApplicationProperties::setId(const QString& id)
{
    d->id = id;
}

QString ApplicationProperties::name() const
{
    return d->name;
}

void ApplicationProperties::setName(const QString& name)
{
    d->name = name;
}

QString ApplicationProperties::organization() const
{
    return d->organization;
}

void ApplicationProperties::setOrganization(const QString& organization)
{
    d->organization = organization;
}

QString ApplicationProperties::version() const
{
    return d->version;
}

void ApplicationProperties::setVersion(const QString &version)
{
    d->version = version;
}

}
