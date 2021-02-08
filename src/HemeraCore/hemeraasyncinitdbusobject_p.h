#ifndef HEMERA_HEMERAASYNCINITDBUSOBJECT_P_H
#define HEMERA_HEMERAASYNCINITDBUSOBJECT_P_H

#include <HemeraCore/AsyncInitDBusObject>

#include <private/HemeraCore/hemeraasyncinitobject_p.h>

namespace Hemera
{

class AsyncInitDBusObjectPrivate : public AsyncInitObjectPrivate
{
public:
    AsyncInitDBusObjectPrivate(AsyncInitObject* q) : AsyncInitObjectPrivate(q) {}
    virtual ~AsyncInitDBusObjectPrivate() {}

    Q_DECLARE_PUBLIC(AsyncInitDBusObject)

    virtual void initHook() Q_DECL_OVERRIDE;

    void propertyChanged();

private:
    const QMetaObject *baseMetaObject() const;
};

}

#endif
