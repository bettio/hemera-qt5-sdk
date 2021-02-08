/*
 *
 */

#ifndef HEMERA_GENERATORS_BASEGENERATOR_H
#define HEMERA_GENERATORS_BASEGENERATOR_H

#include <HemeraCore/AsyncInitObject>

class QQmlEngine;
namespace Hemera {
namespace Generators {

class GeneratorPlugin;

class BaseGeneratorPrivate;
class HEMERA_QT5_SDK_EXPORT BaseGenerator : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_h_ptr, BaseGenerator)
    Q_DISABLE_COPY(BaseGenerator)

public:
    explicit BaseGenerator(bool terminateOnCompletion = true, QObject *parent = nullptr);
    virtual ~BaseGenerator();

    uint thingsLeftToDo() const;

    void terminateWithError(const QString &error);

    void setThingsToDo(uint thingsToDo);
    void oneThingLessToDo();

    QByteArray payload(const QString &file);
    void writeFile(const QString &file, const QByteArray &payload);

Q_SIGNALS:
    void completed(bool success);

protected:
    explicit BaseGenerator(BaseGeneratorPrivate &dd, QObject *parent);
};

}
}

#endif // HEMERA_GENERATORS_BASEGENERATOR_H
