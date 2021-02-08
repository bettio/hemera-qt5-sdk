/*
 *
 */

#include "hemerageneratorsbasegenerator_p.h"

#include "hemerageneratorsgeneratorplugin_p.h"

#include <HemeraCore/Literals>

#include <HemeraQml/QmlEngine>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>

#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>

#include <iostream>

#include <hemeractconfig.h>

namespace Hemera {
namespace Generators {

void BaseGeneratorPrivate::exitHook(bool success)
{
    Q_Q(BaseGenerator);

    // We're done.
    Q_EMIT q->completed(success);

    QCoreApplication::processEvents();

    // If we are not supposed to terminate our process, don't do anything.
    if (!terminateOnCompletion) {
        return;
    }

    // Quit
    if (success) {
        QCoreApplication::quit();
    } else {
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

void BaseGeneratorPrivate::initHook()
{
    Hemera::AsyncInitObjectPrivate::initHook();
}

BaseGenerator::BaseGenerator(bool terminateOnCompletion, QObject *parent)
    : BaseGenerator(*new BaseGeneratorPrivate(terminateOnCompletion, this), parent)
{
}

BaseGenerator::BaseGenerator(BaseGeneratorPrivate &dd, QObject *parent)
    : AsyncInitObject(dd, parent)
{
}

BaseGenerator::~BaseGenerator()
{
}

uint BaseGenerator::thingsLeftToDo() const
{
    Q_D(const BaseGenerator);
    return d->thingsToDo;
}

void BaseGenerator::terminateWithError(const QString &error)
{
    Q_D(BaseGenerator);
    std::cerr << error.toLatin1().constData() << '\n';
    d->exitHook(false);
}

void BaseGenerator::oneThingLessToDo()
{
    Q_D(BaseGenerator);

    --d->thingsToDo;
    if (d->thingsToDo == 0) {
        // Happily quit
        d->exitHook(true);
    }
}

void BaseGenerator::setThingsToDo(uint thingsToDo)
{
    Q_D(BaseGenerator);
    d->thingsToDo = thingsToDo;
}

QByteArray BaseGenerator::payload(const QString& file)
{
    QFile fileTemplate(file);

    if (!fileTemplate.open(QIODevice::ReadOnly)){
        terminateWithError(tr("Hemera Generator: cannot open %1 for reading.").arg(fileTemplate.fileName()));
        return QByteArray();
    }

    QByteArray result = fileTemplate.readAll();
    fileTemplate.close();

    return result;
}

void BaseGenerator::writeFile(const QString& file, const QByteArray& payload)
{
    QFile destination(file);

    bool alreadyExists = destination.exists();
    if (!destination.open(QIODevice::WriteOnly)){
        terminateWithError(tr("Hemera Generator: cannot open %1 for writing.").arg(file));
        return;
    }

    bool opOk = true;
    opOk &= destination.write(payload) != -1;
    opOk &= destination.flush();

    // We are not chaning permissions for already existing files
    if (!alreadyExists) {
        // Default permissions, these permissions need to be changed during packaging
        if (!destination.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther)) {
            qDebug() << tr("Warning: cannot set file permissions.");
        }
    }

    destination.close();

    if (!opOk){
        terminateWithError(tr("Hemera Generator: cannot write to %1.").arg(destination.fileName()));
    }
}

}
}
