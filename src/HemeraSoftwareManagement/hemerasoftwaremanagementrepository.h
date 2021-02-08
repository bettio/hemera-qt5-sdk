/*
 *
 */

#ifndef HEMERA_SOFTWAREREPOSITORY_H
#define HEMERA_SOFTWAREREPOSITORY_H

#include <HemeraCore/Global>

#include <QtCore/QJsonObject>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>

class QUrl;

namespace Hemera {

namespace SoftwareManagement {

class RepositoryData;
/**
 * @class Repository
 * @ingroup HemeraSoftwareManagement
 * @headerfile HemeraSoftwareManagement/hemerasoftwaremanagementrepository.h <HemeraSoftwareManagement/Repository>
 *
 * @brief Represents a Software Repository
 *
 * Repository represents an available Software Repository on the system. This class
 * is not meant to be used unless you really know what you are doing.
 *
 * @note Repository is an implicitly shared object.
 *
 * @sa Hemera::SoftwareManagement::SoftwareManager
 */
class HEMERA_QT5_SDK_EXPORT Repository {
public:
    /// Creates an invalid repository
    Repository();
    Repository(const Repository &other);
    ~Repository();

    /// @returns Whether this repository is valid or not
    bool isValid() const;

    Repository &operator=(const Repository &rhs);
    bool operator==(const Repository &other) const;
    inline bool operator!=(const Repository &other) const { return !operator==(other); }

    /// @returns The unique name of this repository
    QString name() const;
    /// @returns The URLs to this repository
    QStringList urls() const;

private:
    QSharedDataPointer<RepositoryData> d;

    explicit Repository(RepositoryData *data);

    friend class Constructors;
};

/**
 * @ingroup HemeraSoftwareManagement
 *
 * Convenience typedef to represent a list of @ref Hemera::SoftwareManagement::Repository
 */
typedef QList< Hemera::SoftwareManagement::Repository > Repositories;

}

}

#endif // HEMERA_SOFTWAREREPOSITORY_H
