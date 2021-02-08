/*
 *
 */

#include "hemerageneratorsbasepropertiesgenerator.h"

#include "hemerageneratorsbasehagenerator_p.h"
#include "hemerageneratorspropertiesplugin.h"
#include "hemerageneratorspropertiesplugin_p.h"

#include <HemeraCore/Literals>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>

#include <hemeractconfig.h>

#include <iostream>

namespace Hemera {
namespace Generators {

class BasePropertiesGeneratorPrivate : public BaseHaGeneratorPrivate
{
public:
    BasePropertiesGeneratorPrivate(const QString &haFile, const QString &sdk, const QString &className, const QString &outputFileName,
                                   bool terminateOnCompletion, BasePropertiesGenerator* q)
        : BaseHaGeneratorPrivate(haFile, terminateOnCompletion, q), sdk(sdk), className(className), outputFileName(outputFileName) {}
    virtual ~BasePropertiesGeneratorPrivate() {}

    Q_DECLARE_PUBLIC(BasePropertiesGenerator)

    virtual void initHook() Q_DECL_OVERRIDE Q_DECL_FINAL;

    virtual void exitHook(bool success) Q_DECL_OVERRIDE Q_DECL_FINAL;

    QString sdk;
    QString className;
    QString outputFileName;
};

void BasePropertiesGeneratorPrivate::exitHook(bool success)
{
    BaseGeneratorPrivate::exitHook(success);
}

void BasePropertiesGeneratorPrivate::initHook()
{
    // Fine, let the init hook do the rest.
    Hemera::Generators::BaseHaGeneratorPrivate::initHook();
}

BasePropertiesGenerator::BasePropertiesGenerator(const QString &haFile, const QString &sdk, const QString &className,
                                                 const QString &outputFileName, bool terminateOnCompletion, QObject* parent)
    : BaseHaGenerator(*new BasePropertiesGeneratorPrivate(haFile, sdk, className, outputFileName, terminateOnCompletion, this), parent)
    , GeneratorPluginManager< PropertiesPlugin, BasePropertiesGenerator >(QLatin1String(StaticConfig::hemeraPropertiesPluginsDir()), this)
{
}

BasePropertiesGenerator::~BasePropertiesGenerator()
{
}

QString BasePropertiesGenerator::className() const
{
    Q_D(const BasePropertiesGenerator);
    return d->className;
}

QString BasePropertiesGenerator::outputFileName() const
{
    Q_D(const BasePropertiesGenerator);
    return d->outputFileName;
}

QString BasePropertiesGenerator::sdk() const
{
    Q_D(const BasePropertiesGenerator);
    return d->sdk;
}

}
}

#include "moc_hemerageneratorsbasepropertiesgenerator.cpp"
