/*
 *
 */

#ifndef HEMERA_QMLNETWORKACCESSMANAGERFACTORY_H
#define HEMERA_QMLNETWORKACCESSMANAGERFACTORY_H

#include <QtQml/QQmlNetworkAccessManagerFactory>

class QQmlEngine;

namespace Hemera {

class QmlNetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    virtual ~QmlNetworkAccessManagerFactory();
    virtual QNetworkAccessManager *create(QObject *parent);

};
}

#endif // HEMERA_QMLNETWORKACCESSMANAGERFACTORY_H
