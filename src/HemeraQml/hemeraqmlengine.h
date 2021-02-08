/*
 *
 */

#ifndef HEMERA_QMLENGINE_H
#define HEMERA_QMLENGINE_H

#include <QtQml/QQmlEngine>

/**
 * @defgroup HemeraQml Hemera QML
 *
 * Hemera QML contains components needed for integrating Hemera applications with QtQuick2 or
 * with QML in general (e.g.: settings).
 *
 * It is contained in the Hemera:: namespace.
 */

namespace Hemera {

/**
 * @ingroup HemeraQml
 * @brief Allocates and returns a QQmlEngine ready to be used in Hemera applications
 *
 * This function acts as an alternative constructor for QQmlEngine. It ensures
 * all of the correct import paths are loaded, and the correct QNetworkAccessManager is
 * used.
 *
 * @note You should care about this function if you need a QQmlEngine outside of
 *       Hemera::QmlGuiApplication. If that is not the case, you should instead use
 *       the already created @ref Hemera::QmlGuiApplication::qmlEngine.
 */
QQmlEngine *qmlEngine(QObject *parent = nullptr);

}

#endif // HEMERA_QMLENGINE_H
