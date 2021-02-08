/*
 *
 */

#ifndef HEMERA_GENERATORS_CONFIGUREPLUGIN_H
#define HEMERA_GENERATORS_CONFIGUREPLUGIN_H

#include <QtCore/QObject>

namespace Hemera {
namespace Generators {

template<class T, class P >
class GeneratorPluginManager;

class BaseConfigure;

class ConfigurePlugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ConfigurePlugin)

public:
    explicit ConfigurePlugin(QObject *parent = nullptr);
    virtual ~ConfigurePlugin();

    virtual bool handleClassName(const char *className) = 0;

protected:
    BaseConfigure *baseConfigure();

private:
    class Private;
    Private * const d;

    friend class GeneratorPluginManager< ConfigurePlugin, BaseConfigure >;
};

}
}

Q_DECLARE_INTERFACE(Hemera::Generators::ConfigurePlugin, "com.ispirata.Hemera.Qt5SDK.Generators.ConfigurePlugin")

#endif // HEMERA_GENERATORS_CONFIGUREPLUGIN_H
