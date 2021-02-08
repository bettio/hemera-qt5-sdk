/*
 *
 */

#include "hemerageneratorsbaseconfigure.h"

#include "hemerageneratorsbasehagenerator_p.h"
#include "hemerageneratorsconfigureplugin.h"
#include "hemerageneratorsconfigureplugin_p.h"

#include <HemeraCore/Literals>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>

#include <hemeractconfig.h>

#include <iostream>

namespace Hemera {
namespace Generators {

class BaseConfigurePrivate : public BaseHaGeneratorPrivate
{
public:
    BaseConfigurePrivate(const QString& haFile, const QString& destDir,
                         bool terminateOnCompletion, BaseConfigure* q) : BaseHaGeneratorPrivate(haFile, terminateOnCompletion, q), destDir(destDir) {}
    virtual ~BaseConfigurePrivate() {}

    Q_DECLARE_PUBLIC(BaseConfigure)

    virtual void initHook() Q_DECL_OVERRIDE Q_DECL_FINAL;

    virtual void exitHook(bool success) Q_DECL_OVERRIDE Q_DECL_FINAL;

    QString destDir;
};

void BaseConfigurePrivate::exitHook(bool success)
{
    if (success) {
        std::cout << "-- " << BaseConfigure::tr("Configuring complete").toStdString() << std::endl;
        std::cout << "-- " << BaseConfigure::tr("Generation complete").toStdString() << std::endl;
    } else {
        std::cout << "-- " << BaseConfigure::tr("Errors occurred, configuring incomplete").toStdString() << std::endl;
    }

    BaseGeneratorPrivate::exitHook(success);
}

void BaseConfigurePrivate::initHook()
{
    Q_Q(BaseConfigure);

    // Check vars

    // check existance of destination directory
    if (!destDir.isEmpty()) {
        QDir dDir;
        if (!dDir.exists(destDir)) {
            q->setInitError(Literals::literal(Literals::Errors::notFound()),
                            BaseConfigure::tr("Directory %1 doesn't exist").arg(destDir));
            return;
        }
    }

    // Fine, let the init hook do the rest.
    Hemera::Generators::BaseHaGeneratorPrivate::initHook();
}

BaseConfigure::BaseConfigure(const QString& haFile,
                             const QString& destDir, bool terminateOnCompletion, QObject* parent)
    : BaseHaGenerator(*new BaseConfigurePrivate(haFile, destDir, terminateOnCompletion, this), parent)
    , GeneratorPluginManager< ConfigurePlugin, BaseConfigure >(QLatin1String(StaticConfig::hemeraConfigurePluginsDir()), this)
{
}

BaseConfigure::~BaseConfigure()
{

}

QString BaseConfigure::destinationDir() const
{
    Q_D(const BaseConfigure);
    return d->destDir;
}

}
}

#include "moc_hemerageneratorsbaseconfigure.cpp"
