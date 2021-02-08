#include "hemeracxxapplicationproperties.h"

#include "hemeracxxproxyqt5applicationproperties.h"

namespace HemeraCxx {

class ApplicationProperties::Private
{
public:
    HemeraCxx::ProxyQt5ApplicationProperties *proxy;
};

ApplicationProperties::ApplicationProperties()
    : d(new Private)
{
    d->proxy = new ProxyQt5ApplicationProperties();
}

ApplicationProperties::~ApplicationProperties()
{
    delete d;
}

Hemera::ApplicationProperties* ApplicationProperties::qt5Object()
{
    return d->proxy;
}

std::string ApplicationProperties::id() const
{
    return d->proxy->id().toStdString();
}

std::string ApplicationProperties::name() const
{
    return d->proxy->name().toStdString();
}

std::string ApplicationProperties::organization() const
{
    return d->proxy->organization().toStdString();
}

std::string ApplicationProperties::version() const
{
    return d->proxy->version().toStdString();
}

void ApplicationProperties::setId(const std::string& id)
{
    d->proxy->setId(QString::fromStdString(id));
}

void ApplicationProperties::setName(const std::string& name)
{
    d->proxy->setName(QString::fromStdString(name));
}

void ApplicationProperties::setOrganization(const std::string& organization)
{
    d->proxy->setOrganization(QString::fromStdString(organization));
}

void ApplicationProperties::setVersion(const std::string& version)
{
    d->proxy->setVersion(QString::fromStdString(version));
}


}
