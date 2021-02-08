#ifndef HEMERA_GENERATORS_GENERATORPLUGIN_P_H
#define HEMERA_GENERATORS_GENERATORPLUGIN_P_H

#include <HemeraGenerators/GeneratorPlugin>

namespace Hemera {
namespace Generators {

class GeneratorPlugin::Private
{
public:
    Private() {}
    virtual ~Private() {}

    BaseHaGenerator *baseGenerator;
};

}
}

#endif
