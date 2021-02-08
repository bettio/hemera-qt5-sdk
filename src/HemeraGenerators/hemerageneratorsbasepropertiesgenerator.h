#ifndef HEMERA_GENERATORS_BASEPROPERTIESGENERATOR_H
#define HEMERA_GENERATORS_BASEPROPERTIESGENERATOR_H

#include <HemeraGenerators/BaseHaGenerator>

#include <HemeraGenerators/PropertiesPlugin>
#include <HemeraGenerators/GeneratorPluginManager>

namespace Hemera {
namespace Generators {

class PropertiesPlugin;

class BasePropertiesGeneratorPrivate;
class BasePropertiesGenerator : public BaseHaGenerator, protected GeneratorPluginManager< PropertiesPlugin, BasePropertiesGenerator >
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_h_ptr, BasePropertiesGenerator)
    Q_DISABLE_COPY(BasePropertiesGenerator)

public:
    explicit BasePropertiesGenerator(const QString &haFile, const QString &sdk, const QString &className,
                                     const QString &outputFileName, bool terminateOnCompletion = true, QObject *parent = nullptr);
    virtual ~BasePropertiesGenerator();

    QString sdk() const;
    QString className() const;
    QString outputFileName() const;
};
}
}

#endif // HEMERA_GENERATORS_BASEPROPERTIESGENERATOR_H
