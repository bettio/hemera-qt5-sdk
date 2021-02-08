/*
 *
 */

#include "hemeraqmlnetworkaccessmanagerfactory.h"

#include "hemeranetworkaccessmanager.h"

namespace Hemera
{

QmlNetworkAccessManagerFactory::~QmlNetworkAccessManagerFactory()
{

}

QNetworkAccessManager *QmlNetworkAccessManagerFactory::create(QObject* parent)
{
    return new Hemera::NetworkAccessManager(parent);
}

}
