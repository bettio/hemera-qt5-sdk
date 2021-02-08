/*
 *
 */

#ifndef HEMERA_DECLARATIVEENGINE_H
#define HEMERA_DECLARATIVEENGINE_H

#include <QtDeclarative/QDeclarativeEngine>

namespace Hemera {

/**
 * @ingroup HemeraWidgets
 * @brief Allocates and returns a QDeclarativeEngine ready to be used in Hemera applications
 *
 * This function acts as an alternative constructor for QDeclarativeEngine. It ensures
 * all of the correct import paths are loaded, and the correct QNetworkAccessManager is
 * used.
 *
 * @note You should care about this function if you need a QDeclarativeEngine outside of
 *       Hemera::QtQuick1Application. If that is not the case, you should instead use
 *       the already created @ref Hemera::QtQuick1Application::declarativeEngine.
 */
QDeclarativeEngine *declarativeEngine(QObject *parent = nullptr);
/**
 * @ingroup HemeraWidgets
 *
 * Helper function for @ref declarativeEngine
 */
void processDeclarativeEngine(QDeclarativeEngine *engine);

}

#endif // HEMERA_DECLARATIVEENGINE_H
