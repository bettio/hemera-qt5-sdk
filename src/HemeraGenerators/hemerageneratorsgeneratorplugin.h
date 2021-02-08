#ifndef HEMERA_GENERATORS_GENERATORPLUGIN_H
#define HEMERA_GENERATORS_GENERATORPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>

namespace Hemera {
namespace Generators {

template<class T, class P >
class GeneratorPluginManager;

class BaseHaGenerator;

class GeneratorPlugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GeneratorPlugin)

public:
    explicit GeneratorPlugin(QObject *parent = nullptr);
    virtual ~GeneratorPlugin();

    virtual QString handlePayloadForClassName(const char *className, const QString &payloadType,
                                              const QVariantMap &arguments = QVariantMap()) = 0;

protected:
    BaseHaGenerator *baseGenerator();

private:
    class Private;
    Private * const d;

    friend class GeneratorPluginManager< GeneratorPlugin, BaseHaGenerator >;
};

}
}

Q_DECLARE_INTERFACE(Hemera::Generators::GeneratorPlugin, "com.ispirata.Hemera.Qt5SDK.Generators.GeneratorPlugin")

#endif // HEMERA_GENERATORS_GENERATORPLUGIN_H
