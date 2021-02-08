#include "applicationlauncherproperties.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>

#include <QtQml/QQmlComponent>

#include <HemeraCore/Literals>
#include <HemeraQml/QmlEngine>
#include <HemeraGenerators/PropertiesPlugin>

#include <hemeractconfig.h>

#include <iostream>

ApplicationLauncherProperties::ApplicationLauncherProperties(const QString &pathToHa)
    : Hemera::Generators::BaseHaGenerator(pathToHa, false)
{
}

ApplicationLauncherProperties::~ApplicationLauncherProperties()
{
}

void ApplicationLauncherProperties::initImpl()
{
    connect(this, &ApplicationLauncherProperties::ready, this, &ApplicationLauncherProperties::writePropertiesClass);

    setThingsToDo(1);
    setReady();
}

void ApplicationLauncherProperties::writePropertiesClass()
{
    setId(haObject()->property("applicationId").toString());
    setName(haObject()->property("name").toString());
    setVersion(haObject()->property("version").toString());
    setOrganization(haObject()->property("organization").toString());

    oneThingLessToDo();
}
