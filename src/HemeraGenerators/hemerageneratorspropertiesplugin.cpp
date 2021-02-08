/*
 *
 */

#include "hemerageneratorspropertiesplugin_p.h"

#include <HemeraGenerators/BasePropertiesGenerator>

namespace Hemera {
namespace Generators {

PropertiesPlugin::PropertiesPlugin(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

PropertiesPlugin::~PropertiesPlugin()
{
    delete d;
}

BasePropertiesGenerator *PropertiesPlugin::baseGenerator()
{
    return d->baseGenerator;
}

}
}
