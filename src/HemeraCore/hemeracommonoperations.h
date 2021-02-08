#ifndef HEMERA_COMMONOPERATIONS_H
#define HEMERA_COMMONOPERATIONS_H

#include <HemeraCore/Global>
#include <HemeraCore/Operation>

#include <QtDBus/QDBusPendingCall>

class QJsonDocument;
class QDBusObjectPath;
class QProcess;

namespace Hemera {

class HEMERA_QT5_SDK_EXPORT SequentialOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(SequentialOperation)

    Q_PRIVATE_SLOT(d, void finishedOp(Hemera::Operation *))

public:
    enum SequenceOption {
        NoSequenceOptions = 0,
        ContinueOnFailure = 1
    };
    Q_DECLARE_FLAGS(SequenceOptions, SequenceOption)
    Q_FLAGS(SequenceOptions)
    Q_ENUM(SequenceOption)

    explicit SequentialOperation(const QList<Hemera::Operation*> &sequence, QObject *parent = nullptr);
    explicit SequentialOperation(const QList<Hemera::Operation*> &sequence, const QList<Hemera::Operation*> &revertSequence, QObject *parent = nullptr);
    explicit SequentialOperation(const QList<Hemera::Operation*> &sequence, const QList<Hemera::Operation*> &revertSequence, SequenceOptions options, ExecutionOptions execOptions = NoOptions, QObject *parent = nullptr);
    virtual ~SequentialOperation();

    bool isRunningCleanup() const;
    bool hasCleanupSucceeded() const;

Q_SIGNALS:
    void stepFinished(Hemera::Operation *op);
    void stepFailed(Hemera::Operation *op);
    void cleanupStarted();
    void cleanupSucceeded();
    void cleanupFailed(Hemera::Operation *failedStep);

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT CompositeOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(CompositeOperation)

public:
    enum CompositeOption {
        NoCompositeOptions = 0,
        DontAbortOnFailure = 1
    };
    Q_DECLARE_FLAGS(CompositeOptions, CompositeOption)
    Q_FLAGS(CompositeOptions)
    Q_ENUM(CompositeOption)

    explicit CompositeOperation(const QList<Hemera::Operation*> &operations, QObject *parent = nullptr);
    explicit CompositeOperation(const QList<Hemera::Operation*> &operations, CompositeOptions options,
                                ExecutionOptions execOptions = NoOptions, QObject *parent = nullptr);
    virtual ~CompositeOperation();

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

/**
 * QProcess should be an already configured, not started QProcess instance, ready to be started
 * with start() (with no arguments).
 */
class HEMERA_QT5_SDK_EXPORT ProcessOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(ProcessOperation)

public:
    explicit ProcessOperation(QProcess *process, QObject *parent = nullptr);
    virtual ~ProcessOperation();

    QProcess *process() const;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT FailureOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(FailureOperation)

public:
    explicit FailureOperation(const QString &errorName, const QString &errorMessage, QObject *parent = nullptr);
    virtual ~FailureOperation();

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT SuccessOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(SuccessOperation)

public:
    explicit SuccessOperation(QObject *parent = nullptr);
    virtual ~SuccessOperation();

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusVoidOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusVoidOperation)

public:
    explicit DBusVoidOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusVoidOperation();

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};


class HEMERA_QT5_SDK_EXPORT ObjectOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(ObjectOperation)

    Q_PROPERTY(QObject * result READ result)

public:
    virtual ~ObjectOperation();
    virtual QObject *result() const = 0;

protected:
    explicit ObjectOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};


class HEMERA_QT5_SDK_EXPORT VariantOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(VariantOperation)

    Q_PROPERTY(QVariant result READ result)

public:
    virtual ~VariantOperation();
    virtual QVariant result() const = 0;

protected:
    explicit VariantOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};


class HEMERA_QT5_SDK_EXPORT DBusVariantOperation : public Hemera::VariantOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusVariantOperation)

    Q_PROPERTY(QVariant result READ result)

public:
    DBusVariantOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusVariantOperation();

    virtual QVariant result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};


class HEMERA_QT5_SDK_EXPORT VariantMapOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(VariantMapOperation)

    Q_PROPERTY(QVariantMap result READ result)

public:
    virtual ~VariantMapOperation();
    virtual QVariantMap result() const = 0;

protected:
    explicit VariantMapOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};


class HEMERA_QT5_SDK_EXPORT DBusVariantMapOperation : public Hemera::VariantMapOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusVariantMapOperation)

    Q_PROPERTY(QVariantMap result READ result)

public:
    DBusVariantMapOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusVariantMapOperation();

    virtual QVariantMap result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT BoolOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(BoolOperation)

    Q_PROPERTY(bool result READ result)

public:
    virtual ~BoolOperation();
    virtual bool result() const = 0;

protected:
    BoolOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusBoolOperation : public Hemera::BoolOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusBoolOperation)

    Q_PROPERTY(bool result READ result)

public:
    DBusBoolOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusBoolOperation();

    virtual bool result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT UIntOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(UIntOperation)

    Q_PROPERTY(uint result READ result)

public:
    virtual ~UIntOperation();
    virtual uint result() const = 0;

protected:
    UIntOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusUIntOperation : public Hemera::UIntOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusUIntOperation)

    Q_PROPERTY(uint result READ result)

public:
    DBusUIntOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusUIntOperation();

    virtual uint result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT ByteArrayOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(ByteArrayOperation)

    Q_PROPERTY(QByteArray result READ result)

public:
    virtual ~ByteArrayOperation();
    virtual QByteArray result() const = 0;

protected:
    ByteArrayOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusByteArrayOperation : public Hemera::ByteArrayOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusByteArrayOperation)

    Q_PROPERTY(QByteArray result READ result)

public:
    DBusByteArrayOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusByteArrayOperation();

    virtual QByteArray result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT SSLCertificateOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(SSLCertificateOperation)

    Q_PROPERTY(QByteArray privateKey READ privateKey)
    Q_PROPERTY(QByteArray certificate READ certificate)

public:
    virtual ~SSLCertificateOperation();
    virtual QByteArray privateKey() const = 0;
    virtual QByteArray certificate() const = 0;

protected:
    SSLCertificateOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusSSLCertificateOperation : public Hemera::SSLCertificateOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusSSLCertificateOperation)

    Q_PROPERTY(QByteArray privateKey READ privateKey)
    Q_PROPERTY(QByteArray certificate READ certificate)

public:
    DBusSSLCertificateOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusSSLCertificateOperation();

    virtual QByteArray privateKey() const override;
    virtual QByteArray certificate() const override;

protected Q_SLOTS:
    virtual void startImpl() override final;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT StringOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(StringOperation)

    Q_PROPERTY(QString result READ result)

public:
    virtual ~StringOperation();
    virtual QString result() const = 0;

protected:
    StringOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusStringOperation : public Hemera::StringOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusStringOperation)

    Q_PROPERTY(QString result READ result)

public:
    DBusStringOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusStringOperation();

    virtual QString result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};


class HEMERA_QT5_SDK_EXPORT StringListOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(StringListOperation)

    Q_PROPERTY(QStringList result READ result)

public:
    virtual ~StringListOperation();
    virtual QStringList result() const = 0;

protected:
    StringListOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusStringListOperation : public Hemera::StringListOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusStringListOperation)

    Q_PROPERTY(QStringList result READ result)

public:
    DBusStringListOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusStringListOperation();

    virtual QStringList result() const override;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT JsonOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(JsonOperation)

    Q_PROPERTY(QJsonDocument result READ result)

public:
    virtual ~JsonOperation();
    virtual QJsonDocument result() const = 0;

protected:
    JsonOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusJsonOperation : public Hemera::JsonOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusJsonOperation)

    Q_PROPERTY(QJsonDocument result READ result)

public:
    DBusJsonOperation(const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusJsonOperation();

    QJsonDocument result() const;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT DBusObjectPathOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusObjectPathOperation )

    Q_PROPERTY(QDBusObjectPath result READ result)

public:
    DBusObjectPathOperation (const QDBusPendingCall &call, QObject *parent = nullptr);
    virtual ~DBusObjectPathOperation ();

    QDBusObjectPath result() const;

protected Q_SLOTS:
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    class Private;
    Private * const d;
};

class HEMERA_QT5_SDK_EXPORT UrlOperation : public Hemera::Operation
{
    Q_OBJECT
    Q_DISABLE_COPY(UrlOperation)

    Q_PROPERTY(QUrl result READ result)

public:
    virtual ~UrlOperation();
    virtual QUrl result() const = 0;

protected:
    UrlOperation(QObject *parent = nullptr);

private:
    class Private;
    Private * const d;
};

}

#endif // HEMERA_DBUSVOIDOPERATION_H
