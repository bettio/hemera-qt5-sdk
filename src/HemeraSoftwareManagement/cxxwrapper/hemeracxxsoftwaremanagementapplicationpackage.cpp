#include "hemeracxxsoftwaremanagementapplicationpackage_p.h"

#include <QtCore/QSharedData>
#include <QtCore/QUrl>

namespace HemeraCxx {
namespace SoftwareManagement {

Hemera::SoftwareManagement::ApplicationPackages ApplicationPackageData::fromCxx(const HemeraCxx::SoftwareManagement::ApplicationPackages &packages)
{
    Hemera::SoftwareManagement::ApplicationPackages ret;
    // Convert
    for (const HemeraCxx::SoftwareManagement::ApplicationPackage &package : packages) {
        ret.push_back(package.d->proxy);
    }
    return ret;
}

HemeraCxx::SoftwareManagement::ApplicationPackages ApplicationPackageData::toCxx(const Hemera::SoftwareManagement::ApplicationPackages &packages)
{
    HemeraCxx::SoftwareManagement::ApplicationPackages ret;
    // Convert
    for (const Hemera::SoftwareManagement::ApplicationPackage &package : packages) {
        ret.push_back(HemeraCxx::SoftwareManagement::ApplicationPackage(package));
    }
    return ret;
}


ApplicationPackage::ApplicationPackage()
    : d(new ApplicationPackageData())
{
}

ApplicationPackage::ApplicationPackage(ApplicationPackageData* data)
    : d(data)
{
}

ApplicationPackage::ApplicationPackage(const Hemera::SoftwareManagement::ApplicationPackage &other)
    : d(new ApplicationPackageData(other))
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
    return d->proxy == other.d->proxy;
}

bool ApplicationPackage::isValid() const
{
    return !d->proxy.isValid();
}

std::string ApplicationPackage::applicationId() const
{
    return d->proxy.applicationId().toStdString();
}

std::string ApplicationPackage::applicationName() const
{
    return d->proxy.applicationName().toStdString();
}

std::string ApplicationPackage::description() const
{
    return d->proxy.description().toStdString();
}

std::string ApplicationPackage::icon() const
{
    return d->proxy.icon().toString().toStdString();
}

bool ApplicationPackage::isInstalled() const
{
    return d->proxy.isInstalled();
}

unsigned long long ApplicationPackage::installedSize() const
{
    return d->proxy.installedSize();
}

std::string ApplicationPackage::packageName() const
{
    return d->proxy.packageName().toStdString();
}

unsigned long long ApplicationPackage::packageSize() const
{
    return d->proxy.packageSize();
}

std::string ApplicationPackage::version() const
{
    return d->proxy.version().toStdString();
}

}
}
