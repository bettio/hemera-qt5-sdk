#include "hemerageneratorscompilerplugin_p.h"

namespace Hemera {
namespace Generators {

CompilerPlugin::CompilerPlugin(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

CompilerPlugin::~CompilerPlugin()
{
    delete d;
}

BaseCompiler* CompilerPlugin::baseCompiler()
{
    return d->baseGenerator;
}

}
}
