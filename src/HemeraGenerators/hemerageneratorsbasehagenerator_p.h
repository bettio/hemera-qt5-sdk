#ifndef HEMERA_GENERATORS_BASEHAGENERATOR_P_H
#define HEMERA_GENERATORS_BASEHAGENERATOR_P_H

#include <HemeraGenerators/BaseHaGenerator>

#include "hemerageneratorsbasegenerator_p.h"

class QQmlEngine;
namespace Hemera {
namespace Generators {

class BaseHaGeneratorPrivate : public BaseGeneratorPrivate
{
public:
    BaseHaGeneratorPrivate(const QString &haFile, bool terminateOnCompletion, BaseHaGenerator *q)
        : BaseGeneratorPrivate(terminateOnCompletion, q)
        , haFile(haFile) {}
    virtual ~BaseHaGeneratorPrivate() {}

    Q_DECLARE_PUBLIC(BaseHaGenerator)

    virtual void initHook() Q_DECL_OVERRIDE;

    QObject *ha;
    QString haFile;
    QString haFileName;
    QQmlEngine *qmlEngine;
};

}
}

#endif
