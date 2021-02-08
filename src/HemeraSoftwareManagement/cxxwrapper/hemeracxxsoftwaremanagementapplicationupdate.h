#ifndef HEMERACXX_SOFTWAREMANAGEMENT_APPLICATIONUPDATE_H
#define HEMERACXX_SOFTWAREMANAGEMENT_APPLICATIONUPDATE_H

#include <HemeraCore/Global>
#include <HemeraCxxCore/Global>

#include <QtCore/QSharedDataPointer>

namespace Hemera {
namespace SoftwareManagement {
class ApplicationUpdate;
}
}

namespace HemeraCxx {
namespace SoftwareManagement {

class SoftwareManagerPrivate;

class ApplicationUpdateData;
class HEMERA_QT5_SDK_EXPORT ApplicationUpdate {
public:
    ApplicationUpdate();
    ApplicationUpdate(const Hemera::SoftwareManagement::ApplicationUpdate &other);
    ApplicationUpdate(const ApplicationUpdate &other);
    ~ApplicationUpdate();

    ApplicationUpdate &operator=(const ApplicationUpdate &rhs);
    bool operator==(const ApplicationUpdate &other) const;
    inline bool operator!=(const ApplicationUpdate &other) const { return !operator==(other); }

    bool isValid() const;

    std::string applicationId() const;
    std::string applicationName() const;
    std::string description() const;
    std::string installedVersion() const;
    std::string version() const;
    int downloadSize() const;
    int installedSize() const;
    std::string changelog() const;

private:
    QSharedDataPointer<ApplicationUpdateData> d;

    explicit ApplicationUpdate(ApplicationUpdateData *data);

    friend class ApplicationUpdateData;
};

typedef std::vector< HemeraCxx::SoftwareManagement::ApplicationUpdate > ApplicationUpdates;

// Wrappers
typedef ApplicationUpdate ApplicationUpdateWrapper;
typedef ApplicationUpdates ApplicationUpdatesWrapper;

}

}

#endif // HEMERA_APPLICATIONUPDATE_H
