#ifndef HEMERA_GENERATORS_BASECOMPILER_H
#define HEMERA_GENERATORS_BASECOMPILER_H

#include <HemeraGenerators/BaseGenerator>
#include <HemeraGenerators/GeneratorPluginManager>

#include <QtCore/QCoreApplication>
#include <QtCore/QSharedDataPointer>

namespace Hemera {
namespace Generators {

class CompilerPlugin;

class CompilerConfigurationData;
class CompilerConfiguration {
    Q_DECLARE_TR_FUNCTIONS(CompilerConfiguration)
public:
    CompilerConfiguration();
    CompilerConfiguration(const CompilerConfiguration &other);
    ~CompilerConfiguration();

    CompilerConfiguration &operator=(const CompilerConfiguration &rhs);
    bool operator==(const CompilerConfiguration &other) const;
    inline bool operator!=(const CompilerConfiguration &other) const { return !operator==(other); }

    bool isValid() const;

    bool allowMultilink() const;
    bool cleanup() const;
    bool cleanupAllPrevious() const;
    bool inTreeCompile() const;
    bool invokeUserManager() const;
    bool debugSupport() const;
    QStringList files() const;
    QString prefix() const;

private:
    CompilerConfiguration(const QStringList &arguments);

    QSharedDataPointer<CompilerConfigurationData> d;

    friend class BaseCompilerPrivate;
};

class BaseCompilerPrivate;
class BaseCompiler : public BaseGenerator, private GeneratorPluginManager< CompilerPlugin, BaseCompiler >
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_h_ptr, BaseCompiler)
    Q_DISABLE_COPY(BaseCompiler)

    Q_PRIVATE_SLOT(d_func(), void compileNext())

public:
    explicit BaseCompiler(const QStringList &arguments = QCoreApplication::arguments(), bool terminateOnCompletion = true, QObject* parent = nullptr);
    virtual ~BaseCompiler();

    QQmlEngine *qmlEngine();

    CompilerConfiguration configuration() const;

protected:
    void startCompilation();
};
}
}

#endif // HEMERA_GENERATORS_BASECOMPILER_H
