#ifndef APPLICATIONLAUNCHERPROPERTIES_H
#define APPLICATIONLAUNCHERPROPERTIES_H

#include <HemeraCore/ApplicationProperties>

#include <HemeraGenerators/BaseHaGenerator>

class ApplicationLauncherProperties : public Hemera::Generators::BaseHaGenerator, public Hemera::ApplicationProperties
{
    Q_OBJECT
public:
    explicit ApplicationLauncherProperties(const QString &pathToHa);
    virtual ~ApplicationLauncherProperties();

public Q_SLOTS:
    void writePropertiesClass();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE;
};

#endif // APPLICATIONLAUNCHERPROPERTIES_H
