#ifndef HEMERACXX_SOFTWAREMANAGEMENT_APPLICATIONPACKAGE_H
#define HEMERACXX_SOFTWAREMANAGEMENT_APPLICATIONPACKAGE_H

#include <QtCore/QSharedDataPointer>

namespace Hemera {
namespace SoftwareManagement {
class ApplicationPackage;
}
}

namespace HemeraCxx {
namespace SoftwareManagement {

class SoftwareManagerPrivate;

class ApplicationPackageData;
class ApplicationPackage
{
public:
    ApplicationPackage();
    ApplicationPackage(const ApplicationPackage &other);
    ApplicationPackage(const Hemera::SoftwareManagement::ApplicationPackage &other);
    ~ApplicationPackage();

    ApplicationPackage &operator=(const ApplicationPackage &rhs);
    bool operator==(const ApplicationPackage &other) const;
    inline bool operator!=(const ApplicationPackage &other) const { return !operator==(other); }

    bool isValid() const;

    std::string applicationId() const;
    std::string applicationName() const;
    std::string description() const;
    std::string icon() const;
    std::string packageName() const;
    std::string version() const;
    unsigned long long packageSize() const;
    unsigned long long installedSize() const;
    bool isInstalled() const;

private:
    QSharedDataPointer<ApplicationPackageData> d;

    explicit ApplicationPackage(ApplicationPackageData *data);

    friend class ApplicationPackageData;
};

typedef std::vector< HemeraCxx::SoftwareManagement::ApplicationPackage > ApplicationPackages;

// Wrappers
typedef ApplicationPackage ApplicationPackageWrapper;
typedef ApplicationPackages ApplicationPackagesWrapper;

}
}

#endif // HEMERACXX_SOFTWAREMANAGEMENT_APPLICATIONPACKAGE_H
