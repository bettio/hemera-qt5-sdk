/*
 *
 */

#ifndef HEMERA_GENERATORS_BASECONFIGURE_H
#define HEMERA_GENERATORS_BASECONFIGURE_H

#include <HemeraGenerators/BaseHaGenerator>

#include <HemeraGenerators/ConfigurePlugin>
#include <HemeraGenerators/GeneratorPluginManager>

namespace Hemera {
namespace Generators {

class BaseConfigurePrivate;
class BaseConfigure : public BaseHaGenerator, protected GeneratorPluginManager< ConfigurePlugin, BaseConfigure >
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_h_ptr, BaseConfigure)
    Q_DISABLE_COPY(BaseConfigure)

public:
    explicit BaseConfigure(const QString &haFile,
                           const QString &destDir = QString(), bool terminateOnCompletion = true,
                           QObject *parent = nullptr);
    virtual ~BaseConfigure();

    QString destinationDir() const;
};
}
}

#endif // HEMERA_GENERATORS_BASECONFIGURE_H
