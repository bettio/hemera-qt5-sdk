#ifndef HEMERA_GENERATORS_COMPILERPLUGIN_H
#define HEMERA_GENERATORS_COMPILERPLUGIN_H

#include <QtCore/QObject>

#include <HemeraGenerators/BaseCompiler>

namespace Hemera {

class Operation;
namespace Generators {

template<class T, class P >
class GeneratorPluginManager;

class BaseCompiler;

class CompilerPlugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CompilerPlugin)

public:
    explicit CompilerPlugin(QObject *parent = nullptr);
    virtual ~CompilerPlugin();

    virtual Hemera::Operation *operationForClassName(const char *className, QObject *configurationObject,
                                                     const CompilerConfiguration &compilerConfiguration) = 0;

protected:
    BaseCompiler *baseCompiler();

private:
    class Private;
    Private * const d;

    friend class GeneratorPluginManager< CompilerPlugin, BaseCompiler >;
};
}
}

Q_DECLARE_INTERFACE(Hemera::Generators::CompilerPlugin, "com.ispirata.Hemera.Qt5SDK.Generators.CompilerPlugin")

#endif // HEMERA_GENERATORS_COMPILERPLUGIN_H
