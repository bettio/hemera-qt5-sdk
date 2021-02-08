/*
 *
 */

#ifndef HEMERA_GENERATORS_BASEHAGENERATOR_H
#define HEMERA_GENERATORS_BASEHAGENERATOR_H

#include <HemeraGenerators/BaseGenerator>

class QQmlEngine;
namespace Hemera {
namespace Generators {

class GeneratorPlugin;

class BaseHaGeneratorPrivate;
class HEMERA_QT5_SDK_EXPORT BaseHaGenerator : public BaseGenerator
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_h_ptr, BaseHaGenerator)
    Q_DISABLE_COPY(BaseHaGenerator)

public:
    explicit BaseHaGenerator(const QString &haFile, bool terminateOnCompletion = true, QObject *parent = nullptr);
    virtual ~BaseHaGenerator();

    QString haFileName() const;
    QObject *haObject() const;
    QQmlEngine *haQmlEngine() const;

protected:
    explicit BaseHaGenerator(BaseHaGeneratorPrivate &dd, QObject *parent);
};

}
}

#endif // HEMERA_GENERATORS_BASEGENERATOR_H
