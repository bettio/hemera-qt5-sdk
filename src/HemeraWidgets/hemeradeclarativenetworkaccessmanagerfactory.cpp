/*
 *
 */

#include "hemeradeclarativenetworkaccessmanagerfactory.h"

#include <HemeraCore/NetworkAccessManager>

namespace Hemera
{

DeclarativeNetworkAccessManagerFactory::~DeclarativeNetworkAccessManagerFactory()
{

}

QNetworkAccessManager *DeclarativeNetworkAccessManagerFactory::create(QObject* parent)
{
    return new Hemera::NetworkAccessManager(parent);
}

}
