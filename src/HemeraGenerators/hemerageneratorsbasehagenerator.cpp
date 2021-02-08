/*
 *
 */

#include "hemerageneratorsbasehagenerator_p.h"

#include <HemeraCore/Literals>

#include <HemeraQml/QmlEngine>

#include <QtCore/QCoreApplication>
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

void BaseHaGeneratorPrivate::initHook()
{
    Q_Q(BaseHaGenerator);

    if (!QFile::exists(haFile)) {
        q->setInitError(Literals::literal(Literals::Errors::notFound()),
                        BaseHaGenerator::tr("File %1 does not exist").arg(haFile));
        return;
    }

    qmlEngine = Hemera::qmlEngine(q);
    haFileName = haFile.split(QLatin1Char('/')).last();

    // Activate Orbit Handler
    QQmlComponent *haComponent = new QQmlComponent(qmlEngine, haFile, QQmlComponent::Asynchronous, q);

    auto onComponentStatusChanged = [this, haComponent, q] (QQmlComponent::Status status) {
        if (status == QQmlComponent::Error) {
            q->setInitError(Literals::literal(Literals::Errors::declarativeError()),
                            [haComponent] () -> QString {
                                QString result;
                                for (const QQmlError &error : haComponent->errors()) {
                                    result.append(error.toString());
                                    result.append(QLatin1Char('\n'));
                                }
                            return result; } ());
            haComponent->deleteLater();
        } else if (status == QQmlComponent::Ready) {
            ha = haComponent->create();
            if (ha == nullptr) {
                q->setInitError(Literals::literal(Literals::Errors::declarativeError()),
                                BaseHaGenerator::tr("Could not create component"));
                haComponent->deleteLater();
            } else {
                // Component loaded
                Hemera::Generators::BaseGeneratorPrivate::initHook();
            }
        }
    };

    if (haComponent->isLoading()) {
        QObject::connect(haComponent, &QQmlComponent::statusChanged, onComponentStatusChanged);
    } else {
        onComponentStatusChanged(haComponent->status());
    }
}

BaseHaGenerator::BaseHaGenerator(const QString &haFile, bool terminateOnCompletion, QObject *parent)
    : BaseGenerator(*new BaseHaGeneratorPrivate(haFile, terminateOnCompletion, this), parent)
{
}

BaseHaGenerator::BaseHaGenerator(BaseHaGeneratorPrivate &dd, QObject *parent)
    : BaseGenerator(dd, parent)
{
}

BaseHaGenerator::~BaseHaGenerator()
{
}

QString BaseHaGenerator::haFileName() const
{
    Q_D(const BaseHaGenerator);
    return d->haFileName;
}

QObject *BaseHaGenerator::haObject() const
{
    Q_D(const BaseHaGenerator);
    return d->ha;
}

QQmlEngine* BaseHaGenerator::haQmlEngine() const
{
    Q_D(const BaseHaGenerator);
    return d->qmlEngine;
}

}
}
