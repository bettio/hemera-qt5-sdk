#include "hemeracxxsoftwaremanagementapplicationupdate_p.h"

#include <QtCore/QJsonArray>
#include <QtCore/QSharedData>

namespace HemeraCxx {
namespace SoftwareManagement {

Hemera::SoftwareManagement::ApplicationUpdates ApplicationUpdateData::fromCxx(const HemeraCxx::SoftwareManagement::ApplicationUpdates& updates)
{
    Hemera::SoftwareManagement::ApplicationUpdates ret;
    // Convert
    for (const HemeraCxx::SoftwareManagement::ApplicationUpdate &update : updates) {
        ret.push_back(update.d->proxy);
    }
    return ret;
}

HemeraCxx::SoftwareManagement::ApplicationUpdates ApplicationUpdateData::toCxx(const Hemera::SoftwareManagement::ApplicationUpdates &updates)
{
    HemeraCxx::SoftwareManagement::ApplicationUpdates ret;
    // Convert
    for (const Hemera::SoftwareManagement::ApplicationUpdate &update : updates) {
        ret.push_back(HemeraCxx::SoftwareManagement::ApplicationUpdate(update));
    }
    return ret;
}

ApplicationUpdate::ApplicationUpdate()
    : d(new ApplicationUpdateData())
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

ApplicationUpdate::ApplicationUpdate(const Hemera::SoftwareManagement::ApplicationUpdate& other)
    : d(new ApplicationUpdateData(other))
{
}

ApplicationUpdate::~ApplicationUpdate()
{

}

bool ApplicationUpdate::isValid() const
{
    return d->proxy.isValid();
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
    return d->proxy == other.d->proxy;
}

std::string ApplicationUpdate::applicationId() const
{
    return d->proxy.applicationId().toStdString();
}

std::string ApplicationUpdate::applicationName() const
{
    return d->proxy.applicationName().toStdString();
}

std::string ApplicationUpdate::changelog() const
{
    return d->proxy.changelog().toStdString();
}

std::string ApplicationUpdate::description() const
{
    return d->proxy.description().toStdString();
}

int ApplicationUpdate::downloadSize() const
{
    return d->proxy.downloadSize();
}

int ApplicationUpdate::installedSize() const
{
    return d->proxy.installedSize();
}

std::string ApplicationUpdate::installedVersion() const
{
    return d->proxy.installedVersion().toStdString();
}

std::string ApplicationUpdate::version() const
{
    return d->proxy.version().toStdString();
}

}

}
