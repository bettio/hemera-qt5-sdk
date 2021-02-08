#ifndef HEMERACXX_SOFTWAREMANAGEMENT_SYSTEMUPDATE_H
#define HEMERACXX_SOFTWAREMANAGEMENT_SYSTEMUPDATE_H

#include <HemeraCore/Global>
#include <HemeraCxxCore/Global>

#include <QtCore/QSharedDataPointer>

namespace Hemera {
namespace SoftwareManagement {
class SystemUpdate;
}
}

namespace HemeraCxx {
namespace SoftwareManagement {

class SystemUpdateData;
class HEMERA_QT5_SDK_EXPORT SystemUpdate {
public:
    SystemUpdate();
    SystemUpdate(const SystemUpdate &other);
    SystemUpdate(const Hemera::SoftwareManagement::SystemUpdate &other);
    ~SystemUpdate();

    bool isValid() const;

    SystemUpdate &operator=(const SystemUpdate &rhs);
    bool operator==(const SystemUpdate &other) const;
    inline bool operator!=(const SystemUpdate &other) const { return !operator==(other); }

    std::string applianceVersion() const;
    std::string hemeraVersion() const;
    int downloadSize() const;
    std::string changelog() const;

private:
    QSharedDataPointer<SystemUpdateData> d;

    explicit SystemUpdate(SystemUpdateData *data);

    friend class Constructors;
};

// Wrappers
typedef SystemUpdate SystemUpdateWrapper;

}

}

#endif // HEMERA_SYSTEMUPDATE_H
