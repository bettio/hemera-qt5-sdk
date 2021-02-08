#ifndef HEMERA_HEMERAAPPLICATIONPROPERTIES_H
#define HEMERA_HEMERAAPPLICATIONPROPERTIES_H

#include <QtCore/QString>
#include <HemeraCore/Global>

namespace Hemera {

/**
 * @class ApplicationProperties
 * @ingroup HemeraCore
 * @headerfile Hemera/hemeraapplicationproperties.h <HemeraCore/ApplicationProperties>
 *
 * @brief Base class for application properties
 *
 * This class carries the base application properties. It should never be used directly:
 * instead, it is generated from the .ha file of your application.
 *
 * @sa Application
 */
class HEMERA_QT5_SDK_EXPORT ApplicationProperties
{
    Q_DISABLE_COPY(ApplicationProperties)

public:
    virtual ~ApplicationProperties();

    QString id() const;

    QString name() const;
    QString version() const;
    QString organization() const;

protected:
    explicit ApplicationProperties();

    void setId(const QString &id);

    void setName(const QString &name);
    void setVersion(const QString &version);
    void setOrganization(const QString &organization);

private:
    class Private;
    Private * const d;
};

}

#endif // HEMERA_HEMERAAPPLICATIONPROPERTIES_H
