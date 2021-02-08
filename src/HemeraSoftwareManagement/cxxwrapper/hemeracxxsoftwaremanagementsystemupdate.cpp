#include "hemeracxxsoftwaremanagementsystemupdate.h"

#include <QtCore/QSharedData>

#include <HemeraSoftwareManagement/SystemUpdate>

namespace HemeraCxx {
namespace SoftwareManagement {

class SystemUpdateData : public QSharedData
{
public:
    SystemUpdateData() { }
    SystemUpdateData(const SystemUpdateData &other)
        : QSharedData(other), proxy(other.proxy) { }
    SystemUpdateData(const Hemera::SoftwareManagement::SystemUpdate &data)
        : proxy(data) {}
    ~SystemUpdateData() { }

    Hemera::SoftwareManagement::SystemUpdate proxy;
};

SystemUpdate::SystemUpdate()
    : d(new SystemUpdateData())
{
}

SystemUpdate::SystemUpdate(SystemUpdateData* data)
    : d(data)
{

}

SystemUpdate::SystemUpdate(const Hemera::SoftwareManagement::SystemUpdate& other)
    : d(new SystemUpdateData(other))
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
    return other.d->proxy == d->proxy;
}

std::string SystemUpdate::changelog() const
{
    return d->proxy.changelog().toStdString();
}

int SystemUpdate::downloadSize() const
{
    return d->proxy.downloadSize();
}

bool SystemUpdate::isValid() const
{
    return d->proxy.isValid();
}

std::string SystemUpdate::applianceVersion() const
{
    return d->proxy.applianceVersion().toStdString();
}

std::string SystemUpdate::hemeraVersion() const
{
    return d->proxy.hemeraVersion().toStdString();
}

}

}
