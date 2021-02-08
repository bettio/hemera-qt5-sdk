#include "debughelper.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCall>

#include <hemeractconfig.h>
#include <HemeraCore/Literals>

#include  <sys/types.h>
#include <unistd.h>

#define GDB_EXEC "/usr/bin/gdb"

DebugHelper::DebugHelper(const QString& application, const QString& orbit, const QString &mode, const QString &interface, bool holder, QObject* parent)
    : AsyncInitObject(parent)
    , m_application(application)
    , m_orbit(orbit)
    , m_mode(mode)
    , m_interface(interface)
    , m_holder(holder)
    , m_triesLeft(holder ? 5 : 1)
{
}

DebugHelper::~DebugHelper()
{
}

void DebugHelper::findPidForApplication()
{
    QDir procDir(QStringLiteral("/proc"));
    procDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for (const QString &entry : procDir.entryList()) {
        if (QFile::symLinkTarget(QString::fromLatin1("/proc/%1/exe").arg(entry)) == m_executable) {
            // Match! Now check if the orbit matches. We take another file in /proc which is not a symlink.
            // QFileInfo does not process the ownership of the symlink, but of the target instead. So we take stat.
            QFileInfo procStat(QString::fromLatin1("/proc/%1/stat").arg(entry));
            QString processOwner = procStat.owner();
            if (m_orbit.contains(processOwner.split(QLatin1Char('-')).last())) {
                // Real match! Store the PID and UID.
                m_pid = entry.toInt();
                m_uid = procStat.ownerId();
                m_username = processOwner;
                return;
            }
        }
    }

    m_pid = -1;
}

void DebugHelper::initImpl()
{
    connect(this, &DebugHelper::ready, this, &DebugHelper::startDebugging);

    if (!m_application.isEmpty()) {
        // Let's look for the executable.
        // Ok, now. I know this is ugly as fuck, but I see no other way of doing this.
        QSettings serviceFile(QString::fromLatin1("%1%2%3.service").arg(QLatin1String(Hemera::StaticConfig::systemdUserDir()), QDir::separator(), m_application),
                          QSettings::IniFormat);
            serviceFile.beginGroup(QStringLiteral("Service")); {
            m_executable = serviceFile.value(QStringLiteral("ExecStart")).toString();
        } serviceFile.endGroup();

        setParts(2);

        //check if m_application is a valid pid
        bool isPid = true;
        for (int i = 0; i < m_application.length(); i++){
            isPid &= m_application.at(i).isNumber();
            if (!isPid) break;
        }
        if (!isPid){
            // Try finding pid
            tryFindingPidForApplication();
        } else {
            bool ok;
            m_pid = m_application.toInt(&ok);
            if (ok) {
                setOnePartIsReady();
            }
        }

        setOnePartIsReady();
    }else {
        setReady();
    }

}

void DebugHelper::tryFindingPidForApplication()
{
    findPidForApplication();

    if (m_pid > 0) {
        // Success!
        setOnePartIsReady();
        return;
    }

    --m_triesLeft;
    if (m_triesLeft == 0) {
        setInitError(QLatin1String(Hemera::Literals::Errors::timeout()), tr("Could not find requested application!"));
        return;
    }

    // Retry
    QTimer::singleShot(2000, this, SLOT(tryFindingPidForApplication()));
}

void DebugHelper::startDebugging()
{
    if (m_holder) {
        //We are running in release holder mode

        // Time to release the holder. We need to change a couple things, though...
        setuid(m_uid);
        setenv("XDG_RUNTIME_DIR", qstrdup(QString::fromLatin1("/run/user/%1").arg(m_uid).toLatin1().constData()), 1);
        setenv("DBUS_SESSION_BUS_ADDRESS", qstrdup(QString::fromLatin1("unix:path=/run/user/%1/dbus/user_bus_socket").arg(m_uid).toLatin1().constData()), 1);

        // Wait for the service to come up
        while (!QDBusConnection::sessionBus().interface()->isServiceRegistered(m_application)) {
            QEventLoop e;
            QTimer::singleShot(500, &e, SLOT(quit()));
            e.exec();
        }

        QDBusMessage call = QDBusMessage::createMethodCall(m_application, QLatin1String(Hemera::Literals::DBus::dBusHolderPath()),
                                                               QLatin1String(Hemera::Literals::DBus::dBusHolderInterface()), QStringLiteral("Release"));
        QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(call, 10 * 60 * 1000);

        // We don't care about blocking.
        pendingCall.waitForFinished();

        if (pendingCall.isError()) {
            qWarning() << tr("Failed to release the application holder! The system is likely in an unusable state.") << pendingCall.error();
            QCoreApplication::exit(EXIT_FAILURE);
        } else {
            QCoreApplication::quit();
        }

    } else if (m_mode == QStringLiteral("gdb")) {
        if (m_interface.isEmpty()) {
            execl(GDB_EXEC, qstrdup(m_executable.toLatin1().constData()), "--pid", qstrdup(QString::number(m_pid).toLatin1().constData()), NULL);
        } else {
            execl(GDB_EXEC, qstrdup(m_executable.toLatin1().constData()), "-i", "mi", NULL);
        }
    }
}
