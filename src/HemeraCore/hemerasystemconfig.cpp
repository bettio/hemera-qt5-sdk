#include "hemerafetchsystemconfigoperation.h"
#include "hemerasetsystemconfigoperation.h"

#include "hemeraprivateoperations_p.h"
#include "hemerarootoperationclient_p.h"

#include <QtCore/QLoggingCategory>

#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>

Q_LOGGING_CATEGORY(LOG_HEMERA_SYSTEMCONFIG, "Hemera::SystemConfig")

namespace Hemera
{

class FetchSystemConfigOperationPrivate : public RootOperationClientPrivate
{
public:
    explicit FetchSystemConfigOperationPrivate(FetchSystemConfigOperation *q)
        : RootOperationClientPrivate(q) {}
};

FetchSystemConfigOperation::FetchSystemConfigOperation(const QString &key, QObject *parent)
    : FetchSystemConfigOperation(key, NoOptions, parent)
{
}

FetchSystemConfigOperation::FetchSystemConfigOperation(const QString &key, ExecutionOptions options, QObject *parent)
    : Hemera::RootOperationClient(*new FetchSystemConfigOperationPrivate(this), QStringLiteral("com.ispirata.Hemera.FetchSystemConfigOperation"),
                                  QJsonObject { {QStringLiteral("key"), key} },
                                  options, parent)
{
}

FetchSystemConfigOperation::~FetchSystemConfigOperation()
{
}

QString FetchSystemConfigOperation::value() const
{
    return returnValue().value(QStringLiteral("value")).toString();
}

class SetSystemConfigOperationPrivate : public RootOperationClientPrivate
{
public:
    explicit SetSystemConfigOperationPrivate(SetSystemConfigOperation *q)
        : RootOperationClientPrivate(q) {}
};

SetSystemConfigOperation::SetSystemConfigOperation(const QString &key, const QString &value, QObject *parent)
    : SetSystemConfigOperation(key, value, NoOptions, parent)
{
}

SetSystemConfigOperation::SetSystemConfigOperation(const QString &key, const QString &value, ExecutionOptions options, QObject *parent)
    : Hemera::RootOperationClient(*new SetSystemConfigOperationPrivate(this), QStringLiteral("com.ispirata.Hemera.SetSystemConfigOperation"),
                                  QJsonObject { {QStringLiteral("key"), key}, {QStringLiteral("value"), value} },
                                  options, parent)
{
}

SetSystemConfigOperation::~SetSystemConfigOperation()
{
}

}

#include "moc_hemerafetchsystemconfigoperation.cpp"
#include "moc_hemerasetsystemconfigoperation.cpp"
