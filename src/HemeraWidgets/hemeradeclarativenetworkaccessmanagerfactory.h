/*
 *
 */

#ifndef HEMERA_DECLARATIVENETWORKACCESSMANAGERFACTORY_H
#define HEMERA_DECLARATIVENETWORKACCESSMANAGERFACTORY_H

#include <QtDeclarative/QDeclarativeNetworkAccessManagerFactory>

class QDeclarativeEngine;

namespace Hemera {

QDeclarativeEngine *hemeraDeclarativeEngine(QObject *parent = nullptr);

class DeclarativeNetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    virtual ~DeclarativeNetworkAccessManagerFactory();
    virtual QNetworkAccessManager *create(QObject *parent);

};
}

#endif // HEMERA_DECLARATIVENETWORKACCESSMANAGERFACTORY_H
