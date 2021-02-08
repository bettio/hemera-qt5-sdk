/*
 *
 */

#ifndef HEMERACXX_SOFTWAREMANAGEMENT_SOFTWAREREPOSITORY_H
#define HEMERACXX_SOFTWAREMANAGEMENT_SOFTWAREREPOSITORY_H

#include <HemeraCore/Global>
#include <HemeraCxxCore/Global>

#include <QtCore/QSharedDataPointer>

namespace Hemera {
namespace SoftwareManagement {
class Repository;
}
}

namespace HemeraCxx {
namespace SoftwareManagement {

class RepositoryData;
class HEMERA_QT5_SDK_EXPORT Repository {
public:
    Repository();
    Repository(const Hemera::SoftwareManagement::Repository &other);
    Repository(const Repository &other);
    ~Repository();

    bool isValid() const;

    Repository &operator=(const Repository &rhs);
    bool operator==(const Repository &other) const;
    inline bool operator!=(const Repository &other) const { return !operator==(other); }

    std::string name() const;
    std::vector< std::string > urls() const;

private:
    QSharedDataPointer<RepositoryData> d;

    explicit Repository(RepositoryData *data);
};

typedef std::vector< HemeraCxx::SoftwareManagement::Repository > Repositories;

// Wrappers
typedef Repository RepositoryWrapper;
typedef Repositories RepositoriesWrapper;

}

}

#endif // HEMERA_SOFTWAREREPOSITORY_H
