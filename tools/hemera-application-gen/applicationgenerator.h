#ifndef APPLICATIONGENERATOR_H
#define APPLICATIONGENERATOR_H

#include <QtCore/QObject>

#include <private/HemeraGenerators/hemerageneratorsgeneratorplugin_p.h>
#include <HemeraGenerators/BaseHaGenerator>
#include <HemeraGenerators/GeneratorPluginManager>

class QQmlEngine;

class ApplicationGenerator : public Hemera::Generators::BaseHaGenerator
                           , protected Hemera::Generators::GeneratorPluginManager<Hemera::Generators::GeneratorPlugin,Hemera::Generators::BaseHaGenerator>
{
    Q_OBJECT
public:
    explicit ApplicationGenerator(const QString &haFile, const QString &target, QObject *parent = 0);
    virtual ~ApplicationGenerator();

public Q_SLOTS:
    void writeSystemdServiceFile();
    void writeHemeraServiceFile();
    void writeRootOperationServiceFiles();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE;

private:
    QString m_target;
};

#endif // APPLICATIONGENERATOR_H
