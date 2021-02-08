#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <private/HemeraGenerators/hemerageneratorsconfigureplugin_p.h>
#include <HemeraGenerators/BaseConfigure>
#include <HemeraGenerators/GeneratorPluginManager>

class QQmlEngine;

class Configure : public Hemera::Generators::BaseConfigure
{
    Q_OBJECT

public:
    Configure(const QString &haFile, const QString &destDir = QString(), QObject *parent = nullptr);
    virtual ~Configure();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
};

#endif // CONFIGURE_H
