#ifndef PROPERTIESGENERATOR_H
#define PROPERTIESGENERATOR_H

#include <HemeraGenerators/BasePropertiesGenerator>

namespace Hemera {
namespace Generators {
class PropertiesPlugin;
}
}

class QQmlEngine;

class PropertiesGenerator : public Hemera::Generators::BasePropertiesGenerator
{
    Q_OBJECT

public:
    PropertiesGenerator(const QString &haFile, const QString &sdk, const QString &className,
                        const QString &outputFileName, QObject *parent = nullptr);
    virtual ~PropertiesGenerator();

public Q_SLOTS:
    void writeProperties();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE;
};

#endif // PROPERTIESGENERATOR_H
