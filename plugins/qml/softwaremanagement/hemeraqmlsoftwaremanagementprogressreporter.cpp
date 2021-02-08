#include "hemeraqmlsoftwaremanagementprogressreporter.h"

#include <QtCore/QDebug>

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

namespace Hemera {

namespace Qml {

namespace SoftwareManagement {

class ProgressReporter::Private
{
public:
    Private() {}

    QString errorName;
    QString errorMessage;
};


ProgressReporter::ProgressReporter(QObject* parent)
    : Hemera::SoftwareManagement::ProgressReporter(parent)
    , d(new Private)
{
    // Init
    connect(init(), &Hemera::Operation::finished, this, [this] (Hemera::Operation *op) {
        if (op->isError()) {
            d->errorName = op->errorName();
            d->errorMessage = op->errorMessage();
            Q_EMIT error();
        }
    });
}

ProgressReporter::~ProgressReporter()
{
    delete d;
}

QString ProgressReporter::errorName() const
{
    return d->errorName;
}

QString ProgressReporter::errorMessage() const
{
    return d->errorMessage;
}

}

}

}
