#include "setsystemconfigoperation.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>

#include <HemeraCore/Literals>

SetSystemConfigOperation::SetSystemConfigOperation(const QString &id, QObject *parent)
    : Hemera::RootOperation(id, parent)
{
}

void SetSystemConfigOperation::startImpl()
{
    if (Q_UNLIKELY(!QFile::exists(QStringLiteral("/etc/fw_env.config")))) {
        setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()),
                             tr("Internal error - missing configuration file for private memory area."));
        return;
    }

    QString key = parameters().value(QStringLiteral("key")).toString();
    QString value = parameters().value(QStringLiteral("value")).toString();

    if (Q_UNLIKELY(key.isEmpty() || !parameters().contains(QStringLiteral("value")))) {
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

    m_process->start(QStringLiteral("/usr/sbin/fw_setenv %1 %2").arg(key, value));
}

void SetSystemConfigOperation::onProcessFinished(int exitCode)
{
    if (exitCode != 0) {
        // Process failed. Return error.
        setFinishedWithError(Hemera::Literals::literal(Hemera::Literals::Errors::failedRequest()),
                             tr("Could not set variable %1.").arg(parameters().value(QStringLiteral("key")).toString()));
        return;
    }

    // Great! Let's mark as finished.
    setFinished();
}

ROOT_OPERATION_WORKER(SetSystemConfigOperation, "com.ispirata.Hemera.SetSystemConfigOperation")
