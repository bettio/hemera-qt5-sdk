#ifndef HEMERACXX_SOFTWAREMANAGEMENT_P_APPLICATIONUPDATE_H
#define HEMERACXX_SOFTWAREMANAGEMENT_P_APPLICATIONUPDATE_H

#include <HemeraCxxSoftwareManagement/ApplicationUpdate>

#include <HemeraSoftwareManagement/ApplicationUpdate>

namespace HemeraCxx {
namespace SoftwareManagement {

class ApplicationUpdateData : public QSharedData
{
public:
    ApplicationUpdateData() {}
    ApplicationUpdateData(const ApplicationUpdateData &other)
        : QSharedData(other), proxy(other.proxy) { }
    ApplicationUpdateData(const Hemera::SoftwareManagement::ApplicationUpdate &data)
        : proxy(data) {}
    ~ApplicationUpdateData() { }

    Hemera::SoftwareManagement::ApplicationUpdate proxy;

    static Hemera::SoftwareManagement::ApplicationUpdates fromCxx(const ApplicationUpdates &updates);
    static ApplicationUpdates toCxx(const Hemera::SoftwareManagement::ApplicationUpdates &updates);
};

}

}

#endif // HEMERA_APPLICATIONUPDATE_H
