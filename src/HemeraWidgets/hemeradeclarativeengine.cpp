#include "hemeradeclarativeengine.h"

#include <HemeraWidgets/DeclarativeNetworkAccessManagerFactory>

#include "hemeractconfig.h"

namespace Hemera {

QDeclarativeEngine *declarativeEngine(QObject *parent) {
    QDeclarativeEngine *engine = new QDeclarativeEngine(parent);

    processDeclarativeEngine(engine);

    return engine;
}

void processDeclarativeEngine(QDeclarativeEngine* engine)
{
    // Set NAM
    engine->setNetworkAccessManagerFactory(new DeclarativeNetworkAccessManagerFactory);
    // Add imports dir
    engine->addImportPath(QLatin1String(Hemera::StaticConfig::hemeraQmlImportsPath()));
}

}
