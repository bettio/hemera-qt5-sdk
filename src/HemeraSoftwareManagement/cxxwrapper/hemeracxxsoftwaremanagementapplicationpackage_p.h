#ifndef HEMERACXX_SOFTWAREMANAGEMENT_P_APPLICATIONPACKAGE_H
#define HEMERACXX_SOFTWAREMANAGEMENT_P_APPLICATIONPACKAGE_H

#include <HemeraCxxSoftwareManagement/ApplicationPackage>

#include <HemeraSoftwareManagement/ApplicationPackage>

namespace HemeraCxx {
namespace SoftwareManagement {

class ApplicationPackageData : public QSharedData
{
public:
    ApplicationPackageData() { }
    ApplicationPackageData(const ApplicationPackageData &other)
        : QSharedData(other), proxy(other.proxy) { }
    ApplicationPackageData(const Hemera::SoftwareManagement::ApplicationPackage &data)
        : proxy(data) {}
    ~ApplicationPackageData() { }

    Hemera::SoftwareManagement::ApplicationPackage proxy;

    static Hemera::SoftwareManagement::ApplicationPackages fromCxx(const ApplicationPackages &updates);
    static ApplicationPackages toCxx(const Hemera::SoftwareManagement::ApplicationPackages &updates);
};

}
}

#endif // HEMERACXX_SOFTWAREMANAGEMENT_APPLICATIONPACKAGE_H
