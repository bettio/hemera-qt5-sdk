#include "debugscriptgenerator.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QDebug>

#include <HemeraCore/Literals>
#include <HemeraCore/Operation>
#include <HemeraCore/ServiceManager>

#include <hemeractconfig.h>

DebugScriptGenerator::DebugScriptGenerator(const QString& application, const QString& prefix, const QString& suffix, QObject* parent)
    : AsyncInitObject(parent)
    , m_application(application)
    , m_prefix(prefix)
    , m_suffix(suffix)
{
}

DebugScriptGenerator::~DebugScriptGenerator()
{

}

void DebugScriptGenerator::initImpl()
{
    // Check
    if (m_prefix.isEmpty() && m_suffix.isEmpty()) {
        setInitError(QLatin1String(Hemera::Literals::Errors::badRequest()), tr("Must have at least a suffix or a prefix."));
        return;
    }

    setParts(2);

    // Does the application exist?
    Hemera::ServiceManager *manager = new Hemera::ServiceManager(this);
    connect(manager->init(), &Hemera::Operation::finished, [this, manager] (Hemera::Operation *op) {
            if (op->isError()) {
                setInitError(op->errorName(), tr("Could not initialize Software Manager: %1").arg(op->errorMessage()));
                return;
            }

            // Does the application exist?
            Hemera::ServiceInfo application = manager->findServiceById(m_application);
            if (!application.isValid()) {
                setInitError(QLatin1String(Hemera::Literals::Errors::notFound()),
                             tr("Application %1 does not exist or is not installed").arg(m_application));
                return;
            }

            QString orbitName = application.id();
            orbitName.remove(QLatin1Char('.'));
            // Is the application orbital, with its debug service installed?
            if (!QFile::exists(QStringLiteral("%1%2hemera-orbit-%3_debug.target").arg(QLatin1String(Hemera::StaticConfig::systemdUserDir()),
                                                                                           QDir::separator(), orbitName))) {
                setInitError(QLatin1String(Hemera::Literals::Errors::notFound()),
                             tr("Application %1 is either not orbital or has no debug service installed.").arg(m_application));
                return;

            }

            // Awesome, all is fine!
            setOnePartIsReady();
    });

    connect(this, &DebugScriptGenerator::ready, this, &DebugScriptGenerator::writeDebugScript);
    setOnePartIsReady();
}

void DebugScriptGenerator::writeDebugScript()
{
    // Get the template.
    QFile templateFile(QStringLiteral("%1%2%3").arg(QLatin1String(Hemera::StaticConfig::hemeraDataDir()),
                                                         QDir::separator(), QStringLiteral("hemera_debug_script.sh.in")));

    // If the dir does not exist, create it (we might be on a /var-less Hemera, and this script is intended to be transient).
    {
        QDir dir;
        if (!dir.exists(QLatin1String(Hemera::StaticConfig::debugScriptsDir()))) {
            dir.mkpath(QLatin1String(Hemera::StaticConfig::debugScriptsDir()));
        }
    }

    templateFile.open(QIODevice::ReadOnly);

    QString templateData = QLatin1String(templateFile.readAll());
    if (m_prefix.isEmpty()) {
        templateData.replace(QStringLiteral("@PREFIX@"), QString());
    } else {
        QString prefix = m_prefix;
        // Add trailing space
        prefix.append(QLatin1Char(' '));
        templateData.replace(QStringLiteral("@PREFIX@"), prefix);
    }
    if (m_suffix.isEmpty()) {
        templateData.replace(QStringLiteral("@SUFFIX@"), QString());
    } else {
        QString suffix = m_suffix;
        // Prepend trailing space
        suffix.prepend(QLatin1Char(' '));
        templateData.replace(QStringLiteral("@SUFFIX@"), suffix);
    }

    // Ok, now. I know this is ugly as fuck, but I see no other way of doing this.
    QSettings serviceFile(QStringLiteral("%1%2%3.service").arg(QLatin1String(Hemera::StaticConfig::systemdUserDir()), QDir::separator(), m_application),
                          QSettings::IniFormat);
    serviceFile.beginGroup(QStringLiteral("Service")); {
        templateData.replace(QStringLiteral("@APPLICATION@"), serviceFile.value(QStringLiteral("ExecStart")).toString());
    } serviceFile.endGroup();

    // Good. Now that we have the payload, it's time to write it.
    QFile destFile(QStringLiteral("%1%2%3.sh").arg(QLatin1String(Hemera::StaticConfig::debugScriptsDir()), QDir::separator(), m_application));
    destFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    destFile.write(templateData.toLatin1());
    destFile.flush();
    destFile.setPermissions(QFile::ExeOwner | QFile::ExeGroup | QFile::ExeOther | QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther | QFile::WriteOwner);
    destFile.close();

    // Done.
    QCoreApplication::quit();
}
