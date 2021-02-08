/*
 *
 */

#include "hemerageneratorsconfigureplugin_p.h"

namespace Hemera {
namespace Generators {

ConfigurePlugin::ConfigurePlugin(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

ConfigurePlugin::~ConfigurePlugin()
{
    delete d;
}

BaseConfigure *ConfigurePlugin::baseConfigure()
{
    return d->baseGenerator;
}

}
}
