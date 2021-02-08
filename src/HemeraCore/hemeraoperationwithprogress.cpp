#include "hemeraoperationwithprogress.h"
#include "hemeraoperationwithprogress_p.h"

namespace Hemera {

OperationWithProgress::OperationWithProgress(QObject* parent)
    : OperationWithProgress(NoOptions, parent)
{
}

OperationWithProgress::OperationWithProgress(ExecutionOptions options, QObject* parent)
    : OperationWithProgress(*new OperationWithProgressPrivate(this), NoOptions, parent)
{
}

OperationWithProgress::OperationWithProgress(OperationWithProgressPrivate& dd, ExecutionOptions options, QObject* parent)
    : Operation(dd, options, parent)
{
}

OperationWithProgress::~OperationWithProgress()
{
}

int OperationWithProgress::progress() const
{
    Q_D(const OperationWithProgress);
    return d->progress;
}

QString OperationWithProgress::statusMessage() const
{
    Q_D(const OperationWithProgress);
    return d->statusMessage;
}

void OperationWithProgress::setProgress(int p)
{
    Q_D(OperationWithProgress);

    if (Q_UNLIKELY(p < 0 || p > 100 || d->progress == p)) {
        return;
    }

    d->progress = p;
    Q_EMIT progressChanged(d->progress);
}

void OperationWithProgress::setStatusMessage(const QString& message)
{
    Q_D(OperationWithProgress);

    if (Q_UNLIKELY(d->statusMessage == message)) {
        return;
    }

    d->statusMessage = message;
    Q_EMIT statusMessageChanged(d->statusMessage);
}

}

#include "moc_hemeraoperationwithprogress.cpp"
