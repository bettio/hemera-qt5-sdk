#include "hemeracommonoperations.h"
#include <HemeraCore/Literals>

#include <QtCore/QJsonDocument>
#include <QtCore/QUrl>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>

#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>

#include <functional>

Q_LOGGING_CATEGORY(LOG_SEQUENTIALOPERATION, "Hemera::SequentialOperation")

namespace Hemera {

class SequentialOperation::Private
{
public:
    QList<Hemera::Operation*> sequence;
    QList<Hemera::Operation*> revertSequence;
    QList<Hemera::Operation*>::const_iterator opIterator;
    QString failReasonErrorName;
    QString failReasonErrorMessage;
    SequenceOptions options;
    SequentialOperation *q;
    bool runningRevertSequence : 1;
    bool hasCleanupSucceeded : 1;

    Private(SequentialOperation *parent)
        : q(parent), runningRevertSequence(false), hasCleanupSucceeded(false)
    {
    }

    void finishedOp(Operation *op)
    {
        if (op->isError()) {
            Q_EMIT q->stepFailed(op);
            if (!runningRevertSequence && !(options & ContinueOnFailure) && !revertSequence.isEmpty()) {
                opIterator = revertSequence.constBegin();
                runningRevertSequence = true;
                failReasonErrorName = op->errorName();
                failReasonErrorMessage = op->errorMessage();

                Q_EMIT q->cleanupStarted();
                return;
            } else if (runningRevertSequence || !(options & ContinueOnFailure) ||  revertSequence.isEmpty()) {
                if (runningRevertSequence) {
                    Q_EMIT q->cleanupFailed(op);
                }

                failReasonErrorName = op->errorName();
                failReasonErrorMessage = op->errorMessage();
                q->setFinishedWithError(failReasonErrorName, failReasonErrorMessage);
                return;
            } else {
                Q_EMIT q->stepFinished(op);
                opIterator++;
            }
        } else {
            opIterator++;
        }

        if (opIterator == sequence.constEnd()) {
            q->setFinished();
            return;
        }
        if (opIterator == revertSequence.constEnd()) {
            hasCleanupSucceeded = true;
            Q_EMIT q->cleanupSucceeded();
            q->setFinishedWithError(failReasonErrorName, failReasonErrorMessage);
            return;
        }

        QObject::connect(*opIterator, SIGNAL(finished(Hemera::Operation *)), q, SLOT(finishedOp(Hemera::Operation *)));
        (*opIterator)->start();
    }
};

SequentialOperation::SequentialOperation(const QList<Hemera::Operation*> &sequence, QObject* parent)
    : SequentialOperation(sequence, QList<Hemera::Operation *>(), NoSequenceOptions, NoOptions, parent)
{
}

SequentialOperation::SequentialOperation(const QList<Hemera::Operation*> &sequence, const QList<Hemera::Operation*> &revertSequence, QObject *parent)
    : SequentialOperation(sequence, revertSequence, NoSequenceOptions, NoOptions, parent)
{
}

SequentialOperation::SequentialOperation(const QList<Hemera::Operation*> &sequence, const QList<Hemera::Operation*> &revertSequence,
                                         SequenceOptions options, ExecutionOptions execOptions, QObject *parent)
    : Operation(execOptions, parent)
    , d(new Private(this))
{
    d->sequence = sequence;
    d->revertSequence = revertSequence;
    d->options = options;
    for (const Hemera::Operation *operation : sequence) {
        if (!(operation->executionOptions() & Operation::ExplicitStartOption)) {
            qCWarning(LOG_SEQUENTIALOPERATION) << tr("Executing an Operation without ExplicitStartOption with SequentialOperation can lead to non-deterministic behavior");
        }
    }
    d->opIterator = d->sequence.constBegin();
}

SequentialOperation::~SequentialOperation()
{
    delete d;
}

void SequentialOperation::startImpl()
{
    connect((*d->opIterator), SIGNAL(finished(Hemera::Operation *)), this, SLOT(finishedOp(Hemera::Operation *)));
    (*d->opIterator)->start();
}

bool SequentialOperation::isRunningCleanup() const
{
    return d->runningRevertSequence;
}

bool SequentialOperation::hasCleanupSucceeded() const
{
    return d->hasCleanupSucceeded;
}

/////////////////////


class CompositeOperation::Private
{
public:
    QList<Hemera::Operation*> operations;
    CompositeOptions options;
    uint left;
};

CompositeOperation::CompositeOperation(const QList<Hemera::Operation*> &operations, QObject* parent)
    : CompositeOperation(operations, NoCompositeOptions, NoOptions, parent)
{
}

CompositeOperation::CompositeOperation(const QList<Hemera::Operation*> &operations, CompositeOptions options,
                                       ExecutionOptions execOptions, QObject *parent)
    : Operation(execOptions, parent)
    , d(new Private)
{
    d->options = options;
    d->operations = operations;
    d->left = operations.count();
}

CompositeOperation::~CompositeOperation()
{
    delete d;
}

void CompositeOperation::startImpl()
{
    if (d->left == 0) {
        // Nothing to do
        setFinished();
        return;
    }

    auto process = [this] (Hemera::Operation *operation) {
        if (isError() || isFinished()) {
            return;
        }

        if (operation->isError() && !(d->options & DontAbortOnFailure)) {
            setFinishedWithError(operation->errorName(), operation->errorMessage());
        } else {
            --d->left;
            if (d->left == 0) {
                // Done!
                setFinished();
            }
        }
    };

    for (Hemera::Operation *operation : d->operations) {
        if (operation->isFinished()) {
            process(operation);
        } else {
            connect(operation, &Hemera::Operation::finished, process);
        }
    }
}


/////////////////////

class ProcessOperation::Private
{
public:
    QProcess *process;
};

ProcessOperation::ProcessOperation(QProcess *process, QObject* parent)
    : Operation(parent)
    , d(new Private)
{
    d->process = process;
}

ProcessOperation::~ProcessOperation()
{
}

void ProcessOperation::startImpl()
{
    if (!d->process) {
        setFinishedWithError(Literals::literal(Literals::Errors::badRequest()), QStringLiteral("Passed a null QProcess"));
        return;
    }

    connect(d->process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [this] {
        if (d->process->exitCode() == EXIT_SUCCESS) {
            setFinished();
        } else {
            setFinishedWithError(Literals::literal(Literals::Errors::failedRequest()), QStringLiteral("See QProcess status for details"));
        }
    });
    connect(d->process, &QProcess::errorOccurred, this, [this] (QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            // Process failed. Return error.
            setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::failedRequest()),
                                 QStringLiteral("Failed to start!"));
            return;
        }
    });

    d->process->start();
}

QProcess *ProcessOperation::process() const
{
    return d->process;
}


/////////////////////

class FailureOperation::Private
{
public:
    QString name;
    QString message;
};

FailureOperation::FailureOperation(const QString& errorName, const QString& errorMessage, QObject* parent)
    : Operation(parent)
    , d(new Private)
{
    d->name = errorName;
    d->message = errorMessage;
}

FailureOperation::~FailureOperation()
{
}

void FailureOperation::startImpl()
{
    setFinishedWithError(d->name, d->message);
}

/////////////////////

SuccessOperation::SuccessOperation(QObject* parent)
    : Operation(parent)
    , d(nullptr)
{
}

SuccessOperation::~SuccessOperation()
{
}

void SuccessOperation::startImpl()
{
    setFinished();
}


/////////////////////

class DBusVoidOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
};

DBusVoidOperation::DBusVoidOperation(const QDBusPendingCall& call, QObject* parent)
    : Operation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusVoidOperation::~DBusVoidOperation()
{
    delete d;
}

void DBusVoidOperation::startImpl()
{
    std::function<void(QDBusPendingCallWatcher*)> process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            setFinished();
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

/////////////////////

ObjectOperation::ObjectOperation(QObject *parent)
    : Operation(parent)
    , d(nullptr)
{
}

ObjectOperation::~ObjectOperation()
{
}

/////////////////////

VariantOperation::VariantOperation(QObject *parent)
    : Operation(parent)
    , d(nullptr)
{
}

VariantOperation::~VariantOperation()
{
}

class DBusVariantOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QVariant result;
};

DBusVariantOperation::DBusVariantOperation(const QDBusPendingCall& call, QObject *parent)
    : VariantOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusVariantOperation::~DBusVariantOperation()
{
    delete d;
}

void DBusVariantOperation::startImpl()
{
    std::function<void(QDBusPendingCallWatcher*)> process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QVariant> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QVariant DBusVariantOperation::result() const
{
    return d->result;
}


/////////////////////

VariantMapOperation::VariantMapOperation(QObject *parent)
    : Operation(parent)
    , d(nullptr)
{
}

VariantMapOperation::~VariantMapOperation()
{
}

class DBusVariantMapOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QVariantMap result;
};

DBusVariantMapOperation::DBusVariantMapOperation(const QDBusPendingCall& call, QObject *parent)
    : VariantMapOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusVariantMapOperation::~DBusVariantMapOperation()
{
    delete d;
}

void DBusVariantMapOperation::startImpl()
{
    std::function<void(QDBusPendingCallWatcher*)> process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QVariantMap> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QVariantMap DBusVariantMapOperation::result() const
{
    return d->result;
}

/////////////////////

UIntOperation::UIntOperation(QObject *parent)
    : Operation(parent)
    , d(nullptr)
{
}

UIntOperation::~UIntOperation()
{
}

class DBusUIntOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    uint result;
};

DBusUIntOperation::DBusUIntOperation(const QDBusPendingCall& call, QObject *parent)
    : UIntOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusUIntOperation::~DBusUIntOperation()
{
    delete d;
}

void DBusUIntOperation::startImpl()
{
    std::function<void(QDBusPendingCallWatcher*)> process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<uint> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

uint DBusUIntOperation::result() const
{
    return d->result;
}

/////////////////////

BoolOperation::BoolOperation(QObject *parent)
    : Operation(parent)
    , d(nullptr)
{
}

BoolOperation::~BoolOperation()
{
}

class DBusBoolOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    bool result;
};

DBusBoolOperation::DBusBoolOperation(const QDBusPendingCall& call, QObject *parent)
    : BoolOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusBoolOperation::~DBusBoolOperation()
{
    delete d;
}

void DBusBoolOperation::startImpl()
{
    std::function<void(QDBusPendingCallWatcher*)> process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<bool> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

bool DBusBoolOperation::result() const
{
    return d->result;
}

/////////////////////


ByteArrayOperation::ByteArrayOperation(QObject *parent)
    : Operation(parent),
      d(nullptr)
{
}

ByteArrayOperation::~ByteArrayOperation()
{
}

class DBusByteArrayOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QByteArray result;
};

DBusByteArrayOperation::DBusByteArrayOperation(const QDBusPendingCall& call, QObject* parent)
    : ByteArrayOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusByteArrayOperation::~DBusByteArrayOperation()
{
    delete d;
}

void DBusByteArrayOperation::startImpl()
{
    auto process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QByteArray> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QByteArray DBusByteArrayOperation::result() const
{
    return d->result;
}

/////////////////////


SSLCertificateOperation::SSLCertificateOperation(QObject *parent)
    : Operation(parent)
    , d(nullptr)
{
}

SSLCertificateOperation::~SSLCertificateOperation()
{
}

class DBusSSLCertificateOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QByteArray privateKey;
    QByteArray certificate;
};

DBusSSLCertificateOperation::DBusSSLCertificateOperation(const QDBusPendingCall& call, QObject* parent)
    : SSLCertificateOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusSSLCertificateOperation::~DBusSSLCertificateOperation()
{
    delete d;
}

void DBusSSLCertificateOperation::startImpl()
{
    auto process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QByteArray, QByteArray> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->privateKey = reply.argumentAt<0>();
                d->certificate = reply.argumentAt<1>();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QByteArray DBusSSLCertificateOperation::privateKey() const
{
    return d->privateKey;
}

QByteArray DBusSSLCertificateOperation::certificate() const
{
    return d->certificate;
}

/////////////////////

StringOperation::StringOperation(QObject* parent)
    : Operation(parent)
    , d(nullptr)
{
}

StringOperation::~StringOperation()
{
}

class DBusStringOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QString result;
};

DBusStringOperation::DBusStringOperation(const QDBusPendingCall& call, QObject* parent)
    : StringOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusStringOperation::~DBusStringOperation()
{
    delete d;
}

void DBusStringOperation::startImpl()
{
    auto process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QString> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QString DBusStringOperation::result() const
{
    return d->result;
}


/////////////////////

StringListOperation::StringListOperation(QObject* parent)
    : Operation(parent)
    , d(nullptr)
{
}

StringListOperation::~StringListOperation()
{
}

class DBusStringListOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QStringList result;
};

DBusStringListOperation::DBusStringListOperation(const QDBusPendingCall& call, QObject* parent)
    : StringListOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusStringListOperation::~DBusStringListOperation()
{
    delete d;
}

void DBusStringListOperation::startImpl()
{
    auto process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QStringList> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QStringList DBusStringListOperation::result() const
{
    return d->result;
}

/////////////////////

JsonOperation::JsonOperation(QObject* parent)
    : Operation(parent)
    , d(nullptr)
{
}

JsonOperation::~JsonOperation()
{
}

class DBusJsonOperation::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QJsonDocument result;
};

DBusJsonOperation::DBusJsonOperation(const QDBusPendingCall& call, QObject* parent)
    : JsonOperation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusJsonOperation::~DBusJsonOperation()
{
    delete d;
}

void DBusJsonOperation::startImpl()
{
    auto process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QByteArray> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                QByteArray json = reply.value();
                if (json.isEmpty()) {
                    setFinishedWithError(Literals::literal(Literals::Errors::parseError()), tr("Got an empty payload"));
                } else {
                    QJsonParseError error;
                    d->result = QJsonDocument::fromJson(json, &error);
                    if (error.error != QJsonParseError::NoError) {
                        setFinishedWithError(Literals::literal(Literals::Errors::parseError()), error.errorString());
                    } else {
                        // Everything went good
                        setFinished();
                    }
                }
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QJsonDocument DBusJsonOperation::result() const
{
    return d->result;
}

/////////////////////

class DBusObjectPathOperation ::Private
{
public:
    QDBusPendingCallWatcher *watcher;
    QDBusObjectPath result;
};

DBusObjectPathOperation::DBusObjectPathOperation(const QDBusPendingCall& call, QObject* parent)
    : Operation(parent)
    , d(new Private)
{
    d->watcher = new QDBusPendingCallWatcher(call, this);
}

DBusObjectPathOperation::~DBusObjectPathOperation()
{
    delete d;
}

void DBusObjectPathOperation::startImpl()
{
    auto process = [this] (QDBusPendingCallWatcher *watcher) {
        if (watcher->isError()) {
            setFinishedWithError(watcher->error());
        } else {
            QDBusPendingReply<QDBusObjectPath> reply = *watcher;
            if (reply.isError()) {
                setFinishedWithError(reply.error());
            } else {
                d->result = reply.value();
                setFinished();
            }
        }

        watcher->deleteLater();
    };

    if (d->watcher->isFinished()) {
        process(d->watcher);
    } else {
        connect(d->watcher,
                &QDBusPendingCallWatcher::finished,
                process);
    }
}

QDBusObjectPath DBusObjectPathOperation::result() const
{
    return d->result;
}

/////////////////////

class UrlOperation::Private
{
public:
    QUrl result;
};

UrlOperation::UrlOperation(QObject* parent)
    : Operation(parent)
    , d(nullptr)
{
}

UrlOperation::~UrlOperation()
{
    delete d;
}

}

#include "moc_hemeracommonoperations.cpp"
