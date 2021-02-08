#include "propertiesgenerator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>

#include <QtQml/QQmlComponent>

#include <HemeraCore/Literals>
#include <HemeraQml/QmlEngine>

#include <HemeraGenerators/hemerageneratorspropertiesplugin_p.h>

#include <hemeractconfig.h>


#include <iostream>

PropertiesGenerator::PropertiesGenerator(const QString &haFile, const QString &sdk, const QString &className,
                        const QString &outputFileName, QObject* parent)
    : BasePropertiesGenerator(haFile, sdk, className, outputFileName, true, parent)
{
}

PropertiesGenerator::~PropertiesGenerator()
{
}

void PropertiesGenerator::initImpl()
{
    connect(this, &PropertiesGenerator::ready, this, &PropertiesGenerator::writeProperties);

    setThingsToDo(1);
    setReady();
}

void PropertiesGenerator::writeProperties()
{
    for (Hemera::Generators::PropertiesPlugin *plugin : plugins()) {
        if (plugin->propertyGeneration(haObject()->metaObject()->className())) {
            oneThingLessToDo();
            return;
        }
    }

    // Ouch.
}
