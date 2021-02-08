#ifndef HEMERA_GENERATORS_PROPERTIESPLUGIN_P_H
#define HEMERA_GENERATORS_PROPERTIESPLUGIN_P_H

#include <HemeraGenerators/PropertiesPlugin>

class QQmlEngine;
namespace Hemera {
namespace Generators {

class PropertiesPlugin::Private
{
public:
    Private() {}
    virtual ~Private() {}

    BasePropertiesGenerator *baseGenerator;
};

}
}

#endif
