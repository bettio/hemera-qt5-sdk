#include "hemeraqmlengine.h"

#include <HemeraQml/QmlNetworkAccessManagerFactory>

#include "hemeractconfig.h"

namespace Hemera {

QQmlEngine *qmlEngine(QObject *parent) {
    QQmlEngine *engine = new QQmlEngine(parent);
    // Set NAM
    engine->setNetworkAccessManagerFactory(new QmlNetworkAccessManagerFactory);
    // Add imports dir
    engine->addImportPath(QLatin1String(Hemera::StaticConfig::hemeraQmlImportsPath()));

    return engine;
}

}
