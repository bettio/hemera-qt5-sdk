#ifndef HEMERA_GENERATORS_COMPILERPLUGIN_P_H
#define HEMERA_GENERATORS_COMPILERPLUGIN_P_H

#include <HemeraGenerators/CompilerPlugin>

namespace Hemera {
namespace Generators {

class CompilerPlugin::Private
{
public:
    Private() {}
    virtual ~Private() {}

    BaseCompiler *baseGenerator;
};

}
}

#endif
