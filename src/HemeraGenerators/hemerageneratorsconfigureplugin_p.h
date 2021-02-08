#ifndef HEMERA_GENERATORS_CONFIGUREPLUGIN_P_H
#define HEMERA_GENERATORS_CONFIGUREPLUGIN_P_H

#include <HemeraGenerators/ConfigurePlugin>

class QQmlEngine;
namespace Hemera {
namespace Generators {

class ConfigurePlugin::Private
{
public:
    Private() {}
    virtual ~Private() {}

    BaseConfigure *baseGenerator;
};

}
}

#endif
