#include "hemerageneratorsgeneratorplugin_p.h"

#include <HemeraGenerators/BaseHaGenerator>

namespace Hemera {
namespace Generators {

GeneratorPlugin::GeneratorPlugin(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

GeneratorPlugin::~GeneratorPlugin()
{
    delete d;
}

BaseHaGenerator *GeneratorPlugin::baseGenerator()
{
    return d->baseGenerator;
}

}
}
