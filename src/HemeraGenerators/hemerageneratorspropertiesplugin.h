/*
 *
 */

#ifndef HEMERA_GENERATORS_PROPERTIESPLUGIN_H
#define HEMERA_GENERATORS_PROPERTIESPLUGIN_H

#include <QtCore/QObject>

#include <QtCore/QPair>

class PropertiesGenerator;

namespace Hemera {
namespace Generators {

template<class T, class P >
class GeneratorPluginManager;

class BasePropertiesGenerator;

class PropertiesPlugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PropertiesPlugin)

public:
    explicit PropertiesPlugin(QObject* parent = nullptr);
    virtual ~PropertiesPlugin();

    virtual bool propertyGeneration(const char *className) = 0;

protected:
    BasePropertiesGenerator *baseGenerator();

private:
    class Private;
    Private * const d;

    friend class GeneratorPluginManager< PropertiesPlugin, BasePropertiesGenerator >;
};

}
}

Q_DECLARE_INTERFACE(Hemera::Generators::PropertiesPlugin, "com.ispirata.Hemera.Qt5SDK.Generators.PropertiesPlugin")

#endif // HEMERA_GENERATORS_PROPERTIESPLUGIN_H
