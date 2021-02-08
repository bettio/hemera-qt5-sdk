#include "hemeraheadlessapplication.h"

#include "hemeraapplication_p.h"

#include <HemeraCore/ApplicationProperties>

#include <QtCore/QCoreApplication>

namespace Hemera {

class HeadlessApplicationPrivate : public ApplicationPrivate
{
public:
    HeadlessApplicationPrivate(HeadlessApplication *q) : ApplicationPrivate(q) {}
    virtual ~HeadlessApplicationPrivate() {}

    Q_DECLARE_PUBLIC(HeadlessApplication)

    virtual void initHook() Q_DECL_OVERRIDE Q_DECL_FINAL;
};

void HeadlessApplicationPrivate::initHook()
{
    Hemera::ApplicationPrivate::initHook();
}

HeadlessApplication::HeadlessApplication(Hemera::ApplicationProperties *properties)
    : Application(*new HeadlessApplicationPrivate(this), properties)
{
}

HeadlessApplication::~HeadlessApplication()
{
}

}

#include "moc_hemeraheadlessapplication.moc"
