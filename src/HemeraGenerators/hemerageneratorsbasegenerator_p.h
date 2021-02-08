#ifndef HEMERA_GENERATORS_BASEGENERATOR_P_H
#define HEMERA_GENERATORS_BASEGENERATOR_P_H

#include <HemeraGenerators/BaseGenerator>

#include <private/HemeraCore/hemeraasyncinitobject_p.h>

namespace Hemera {
namespace Generators {

class BaseGeneratorPrivate : public AsyncInitObjectPrivate
{
public:
    BaseGeneratorPrivate(bool terminateOnCompletion, BaseGenerator *q)
        : AsyncInitObjectPrivate(q)
        , terminateOnCompletion(terminateOnCompletion) {}
    virtual ~BaseGeneratorPrivate() {}

    Q_DECLARE_PUBLIC(BaseGenerator)

    virtual void initHook() Q_DECL_OVERRIDE;

    virtual void exitHook(bool success);

    uint thingsToDo;
    bool terminateOnCompletion;
};

}
}

#endif
