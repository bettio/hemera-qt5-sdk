#ifndef HEMERA_HEMERAASYNCINITOBJECT_P_H
#define HEMERA_HEMERAASYNCINITOBJECT_P_H

#include <HemeraCore/AsyncInitObject>
#include <HemeraCore/Operation>

#include <QtCore/QPointer>

namespace Hemera
{

class InitObjectOperation : public Operation
{
    Q_OBJECT

public:
    explicit InitObjectOperation(AsyncInitObject *parent);
    virtual ~InitObjectOperation();

protected:
    virtual void startImpl();

private:
    AsyncInitObject *m_object;

    friend class AsyncInitObject;
    friend class AsyncInitObjectPrivate;
};

class AsyncInitObjectPrivate
{
public:
    AsyncInitObjectPrivate(AsyncInitObject *q) : q_ptr(q), ready(false), parts(0), initOperation(nullptr) {}
    virtual ~AsyncInitObjectPrivate() {}

    Q_DECLARE_PUBLIC(AsyncInitObject)
    AsyncInitObject *q_ptr;

    QString errorName;
    QString errorMessage;
    bool ready;
    uint parts;

    QPointer<InitObjectOperation> initOperation;

    virtual void initHook();
    virtual void postInitHook();

private:
    // Q_PRIVATE_SLOT
    void finalizeInit();
};

}

#endif
