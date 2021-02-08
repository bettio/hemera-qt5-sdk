#include "fetchsystemconfigoperation.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>

#include <HemeraCore/Literals>

FetchSystemConfigOperation::FetchSystemConfigOperation(const QString &id, QObject *parent)
    : Hemera::RootOperation(id, parent)
{
}

void FetchSystemConfigOperation::startImpl()
{
    if (Q_UNLIKELY(!QFile::exists(QStringLiteral("/etc/fw_env.config")))) {
        setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()),
                             tr("Internal error - missing configuration file for private memory area."));
        return;
    }

    QString key = parameters().value(QStringLiteral("key")).toString();

    if (Q_UNLIKELY(key.isEmpty())) {
        setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()),
                             tr("Internal error - missing required argument."));
        return;
    }

    m_process = new QProcess(this);
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcessFinished(int)));
    connect(m_process, &QProcess::errorOccurred, this, [this] (QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            // Process failed. Return error.
            setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::failedRequest()),
                                 tr("Failed to start! fw_setenv is probably not installed."));
            return;
        }
    });

    m_process->start(QStringLiteral("/usr/sbin/fw_printenv -n %1").arg(key));
}

void FetchSystemConfigOperation::onProcessFinished(int exitCode)
{
    if (exitCode != 0) {
        // Process failed. Return error.
        if (m_process->readAllStandardError().contains("not defined")) {
            setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::notFound()),
                                 tr("Variable %1 does not exist.").arg(parameters().value(QStringLiteral("key")).toString()));
        } else {
            setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::failedRequest()),
                                 tr("Could not retrieve variable %1.").arg(parameters().value(QStringLiteral("key")).toString()));
        }
        return;
    }

    // Ok! Let's get the result.
    QString output = QString::fromLatin1(m_process->readAllStandardOutput());
    output.remove(QLatin1Char('\n'));

    QJsonObject ret = { {QStringLiteral("value"),output} };
    setReturnValue(ret);
    setFinished();
}

ROOT_OPERATION_WORKER(FetchSystemConfigOperation, "com.ispirata.Hemera.FetchSystemConfigOperation")
