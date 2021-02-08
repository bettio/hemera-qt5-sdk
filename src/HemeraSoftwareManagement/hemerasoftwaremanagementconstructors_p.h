/*
 *
 */

#ifndef HEMERA_SOFTWAREMANAGEMENT_CONSTRUCTORS_H
#define HEMERA_SOFTWAREMANAGEMENT_CONSTRUCTORS_H

#include <HemeraCore/Global>

#include <HemeraSoftwareManagement/ApplicationPackage>
#include <HemeraSoftwareManagement/ApplicationUpdate>
#include <HemeraSoftwareManagement/Repository>
#include <HemeraSoftwareManagement/SystemUpdate>

class QJsonObject;
class QUrl;
namespace Hemera {

namespace SoftwareManagement {

class Constructors {
public:
    // From JSON or data, a mess
    static ApplicationPackage applicationPackageFromData(const QString &applicationId, const QString &applicationName, const QString &description,
                                                         const QUrl &icon, const QString &packageName, const QString &version, quint64 packageSize,
                                                         quint64 installedSize, bool installed);
    static ApplicationPackage applicationPackageFromJson(const QJsonObject &json);
    static ApplicationPackages applicationPackagesFromJson(const QJsonArray &json);
    //ApplicationPackage fromPackage(const QUrl &package);

    static ApplicationUpdate applicationUpdateFromData(const QString &applicationId, const QString &applicationName, const QString &description,
                                                       const QString &installedVersion, const QString &version, quint64 downloadSize,
                                                       qint32 installedSize, const QString &changelog);
    static ApplicationUpdate applicationUpdateFromJson(const QJsonObject &json);
    static ApplicationUpdates applicationUpdatesFromJson(const QJsonArray &json);

    static Repository repositoryFromData(const QString &name, const QStringList &urls);
    static Repository repositoryFromJson(const QJsonObject &json);
    static Repositories repositoriesFromJson(const QJsonArray &json);

    static SystemUpdate systemUpdateFromJson(const QJsonObject &json);


    // To JSON or data. That's easy, overload FTW.
    static QJsonObject toJson(const Hemera::SoftwareManagement::ApplicationPackage &package);
    static QJsonArray toJson(const Hemera::SoftwareManagement::ApplicationPackages &packages);
    static QJsonObject toJson(const Hemera::SoftwareManagement::ApplicationUpdate &update);
    static QJsonArray toJson(const Hemera::SoftwareManagement::ApplicationUpdates &updates);
    static QJsonObject toJson(const Hemera::SoftwareManagement::Repository &repository);
    static QJsonArray toJson(const Hemera::SoftwareManagement::Repositories &repositories);
    static QJsonObject toJson(const Hemera::SoftwareManagement::SystemUpdate &update);
};

}

}

#endif // HEMERA_SOFTWAREMANAGEMENT_CONSTRUCTORS_H
