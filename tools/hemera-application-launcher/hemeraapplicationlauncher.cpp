#include "hemeraapplicationlauncher.h"

#include <QtCore/QUrl>

#include <QtCore/QProcess>
#include <QtCore/QTimer>

#include <HemeraCore/HeadlessApplication>

Q_LOGGING_CATEGORY(DBG_HEMERA_PROXIED_APPLICATION, "Hemera::ProxiedApplication")

HemeraApplicationLauncher::HemeraApplicationLauncher(Hemera::ApplicationProperties *properties, QString executable, QStringList arguments)
    : Hemera::HeadlessApplication(properties)
    , m_executable(executable)
    , m_arguments(arguments)
{
}


HemeraApplicationLauncher::~HemeraApplicationLauncher()
{
}

void HemeraApplicationLauncher::initImpl()
{
    m_process = new QProcess(this);
    m_terminateTimeout = new QTimer(this);

    m_terminateTimeout->setInterval(5000);
    m_terminateTimeout->setSingleShot(true);

    connect(m_terminateTimeout, &QTimer::timeout, [this] {
            // Attempt to kill the process.
            m_process->kill();
    });

    void (QProcess::*finishedSignal)(int exitCode) = &QProcess::finished;
    connect(m_process, finishedSignal, [this] (int exitCode) {
            if (applicationStatus() == Hemera::Application::ApplicationStatus::Stopping) {
                // We can now terminate the proxy
                m_terminateTimeout->stop();
                setStopped();
            } else {
                // Erm. Trouble.
            }
    });

    void (QProcess::*errorSignal)(QProcess::ProcessError error) = &QProcess::error;
    connect(m_process, errorSignal, this, &HemeraApplicationLauncher::onProcessError);

    connect(m_process, &QProcess::readyReadStandardOutput, [this] {
        m_process->setReadChannel(QProcess::StandardOutput);
        qCDebug(DBG_HEMERA_PROXIED_APPLICATION) << processOutput();
    });
    connect(m_process, &QProcess::readyReadStandardError, [this] {
        m_process->setReadChannel(QProcess::StandardError);
        qCWarning(DBG_HEMERA_PROXIED_APPLICATION) << processOutput();
    });

    setReady();
}

void HemeraApplicationLauncher::startImpl()
{
    connect(m_process, &QProcess::started, [this] { setStarted(); });

    m_process->start(m_executable, m_arguments);
}


void HemeraApplicationLauncher::stopImpl()
{
    m_process->terminate();

    m_terminateTimeout->start();
}


void HemeraApplicationLauncher::prepareForShutdown()
{
    setReadyForShutdown();
}

void HemeraApplicationLauncher::onProcessError(QProcess::ProcessError error)
{
    if (applicationStatus() == Hemera::Application::ApplicationStatus::Starting) {
        // Startup failed
        setStarted(Hemera::Application::OperationResult::Fatal);
    } else {
        // Erm. Moar Trouble.
    }
}

