#include "applicationgenerator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include <QtQml/QQmlListReference>

#include <HemeraCore/Literals>
#include <HemeraGenerators/GeneratorPlugin>

#include <hemeractconfig.h>
#include <hemeraservicemanager.h>

ApplicationGenerator::ApplicationGenerator(const QString& haFile, const QString& target, QObject* parent)
    : BaseHaGenerator(haFile, true, parent)
    , GeneratorPluginManager(QLatin1String(Hemera::StaticConfig::hemeraGeneratorPluginsDir()), this)
    , m_target(target)
{
}

ApplicationGenerator::~ApplicationGenerator()
{
}

void ApplicationGenerator::initImpl()
{
    connect(this, &ApplicationGenerator::ready, this, &ApplicationGenerator::writeSystemdServiceFile);
    connect(this, &ApplicationGenerator::ready, this, &ApplicationGenerator::writeHemeraServiceFile);
    connect(this, &ApplicationGenerator::ready, this, &ApplicationGenerator::writeRootOperationServiceFiles);

    setThingsToDo(2);
    setReady();
}

void ApplicationGenerator::writeSystemdServiceFile()
{
    QDir appInstallDir(Hemera::Literals::literal(Hemera::StaticConfig::hemeraApplicationInstallDir()));
    QStringList argumentsService;
    QQmlListReference applicationProfiles(haObject(), "applicationProfiles", haQmlEngine());
    QQmlListReference devices(haObject(), "devices", haQmlEngine());

    // TODO check that any profile class is not specified more than once -> error

    // Key = .service file "key", value = value
    QHash<QString, QString> restartOnFailureVars;
    QHash<QString, QString> watchdogVars;

    for (int i = 0; i < applicationProfiles.count(); ++i) {
        QObject *profile = applicationProfiles.at(i);
        QString profileType = QLatin1String(profile->metaObject()->className());

        if (profileType == QStringLiteral("Hemera::Qml::Settings::RestartOnFailureProfile")) {
            restartOnFailureVars.insert(QStringLiteral("Restart"), QStringLiteral("on-failure"));
            restartOnFailureVars.insert(QStringLiteral("StartLimitInterval"), QString::number(profile->property("interval").toInt()));
            restartOnFailureVars.insert(QStringLiteral("StartLimitAction"), QStringLiteral("reboot"));
            restartOnFailureVars.insert(QStringLiteral("StartLimitBurst"), QString::number(profile->property("maxFailuresPerInterval").toInt()));
        } else if (profileType == QStringLiteral("Hemera::Qml::Settings::WatchdogApplicationProfile")) {
            watchdogVars.insert(QStringLiteral("WatchdogSec"), profile->property("interval").toString());
        }
    }

    QString deviceString;
    for (int i = 0; i < devices.count(); ++i) {
        QObject *device = devices.at(i);

        QString canString;
        QString denyString;
        if (device->property("canRead").toBool()) {
            canString += QLatin1Char('r');
        } else {
            denyString += QLatin1Char('r');
        }
        if (device->property("canWrite").toBool()) {
            canString += QLatin1Char('w');
        } else {
            denyString += QLatin1Char('w');
        }
        if (device->property("canCreate").toBool()) {
            canString += QLatin1Char('m');
        } else {
            denyString += QLatin1Char('m');
        }

        if (!canString.isEmpty()) {
            deviceString += QStringLiteral("DeviceAllow=") + device->property("path").toString() + QLatin1Char(' ') + canString + QLatin1Char('\n');
        }
        if (!denyString.isEmpty()) {
            deviceString += QStringLiteral("DeviceDeny=") + device->property("path").toString() + QLatin1Char(' ') + denyString + QLatin1Char('\n');
        }
    }

    // Get commands from our plugins
    QVariantMap args;
    args.insert(QStringLiteral("target"), m_target);

    QString command;
    QString debugCommand;

    for (Hemera::Generators::GeneratorPlugin *plugin : plugins()) {
        command = plugin->handlePayloadForClassName(haObject()->metaObject()->className(), QStringLiteral("systemd-exec"), args);
        if (!command.isEmpty()) {
            break;
        }
    }
    for (Hemera::Generators::GeneratorPlugin *plugin : plugins()) {
        debugCommand = plugin->handlePayloadForClassName(haObject()->metaObject()->className(), QStringLiteral("systemd-exec-debug"), args);
        if (!debugCommand.isEmpty()) {
            break;
        }
    }

    QString execOptions;

    QString environment = haObject()->property("environment").toString();
    environment.replace(QStringLiteral("@RESOURCE_DIR@"), QString::fromLatin1("%1/%2").arg(QLatin1String(Hemera::StaticConfig::hemeraApplicationBaseResourceDir()),
                                                                                           haObject()->property("applicationId").toString()));

    if (!environment.isEmpty()) {
        execOptions.append(QString::fromLatin1("Environment=\"%1\"\n").arg(environment));
    }

    // More environment from plugins?
    for (Hemera::Generators::GeneratorPlugin *plugin : plugins()) {
        QString ae = plugin->handlePayloadForClassName(haObject()->metaObject()->className(), QStringLiteral("systemd-additional-environment"), args);
        if (!ae.isEmpty()) {
            execOptions.append(QString::fromLatin1("Environment=\"%1\"\n").arg(ae));
        }
    }

    QString workingDirectory = haObject()->property("workingDirectory").toString();
    workingDirectory.replace(QStringLiteral("@RESOURCE_DIR@"), QString::fromLatin1("%1/%2").arg(QLatin1String(Hemera::StaticConfig::hemeraApplicationBaseResourceDir()),
                                                                                                haObject()->property("applicationId").toString()));

    if (!workingDirectory.isEmpty()) {
        execOptions.append(QString::fromLatin1("WorkingDirectory=%1\n").arg(workingDirectory));
    }

    if (!haObject()->property("runAsSpecialUser").toString().isEmpty()) {
        execOptions.append(QString::fromLatin1("User=%1\n").arg(haObject()->property("runAsSpecialUser").toString()));
    }

    // get string to add to the .service file
    auto generateList = [] (const QHash<QString, QString> &varList) -> QString {
        QString sectionStr;
        QHash<QString, QString>::const_iterator it = varList.constBegin();

        while (it != varList.constEnd()) {
            sectionStr += it.key() + QLatin1Char('=') + it.value() + QLatin1Char('\n');
            ++it;
        }

        return sectionStr;
    };

    QString content = QString::fromLatin1(payload(QDir(Hemera::Literals::literal(Hemera::StaticConfig::hemeraDataDir()))
                                                        .absoluteFilePath(QStringLiteral("hemera_systemd.service.in"))))
                    .arg(haObject()->property("name").toString())                               // %1   description
                    .arg(command)                                                               // %2
                    .arg(execOptions)                                                           // %3
                    .arg(generateList(watchdogVars))                                            // %4   watchdog
                    .arg(generateList(restartOnFailureVars))                                    // %5   restart on failure
                    .arg(deviceString);                                                         // %6   devices
    // For debug, we have to add NotifyAccess=all
    if (!execOptions.contains(QStringLiteral("NotifyAccess="))) {
        execOptions.append(QStringLiteral("\nNotifyAccess=all"));
    }
    QString debugContent = QString::fromLatin1(payload(QDir(Hemera::Literals::literal(Hemera::StaticConfig::hemeraDataDir()))
                                                             .absoluteFilePath(QStringLiteral("hemera_systemd.service.in"))))
                    .arg(haObject()->property("name").toString())                               // %1   description
                    .arg(debugCommand)                                                          // %2
                    .arg(execOptions)                                                           // %3
                    // Disable watchdogs.
                    .arg(QString())                                                             // %4   watchdog
                    .arg(generateList(restartOnFailureVars))                                    // %5   restart on failure
                    .arg(deviceString);                                                         // %6   devices

    writeFile(haObject()->property("applicationId").toString() + QStringLiteral(".service"), content.toLatin1());
    writeFile(haObject()->property("applicationId").toString() + QStringLiteral("_debug.service"), debugContent.toLatin1());

    oneThingLessToDo();
}

void ApplicationGenerator::writeHemeraServiceFile()
{
    QJsonObject serviceObject;
    // It's an application.
    serviceObject.insert(QStringLiteral("type"), static_cast<int>(Hemera::ServiceInfo::ServiceType::Application));
    serviceObject.insert(QStringLiteral("handledMimeTypes"), QJsonArray::fromStringList(haObject()->property("handledMimeTypes").toStringList()));

    // Fill the common data
    serviceObject.insert(QStringLiteral("id"), haObject()->property("applicationId").toString());
    serviceObject.insert(QStringLiteral("name"), haObject()->property("name").toString());
    serviceObject.insert(QStringLiteral("description"), haObject()->property("description").toString());
    serviceObject.insert(QStringLiteral("version"), haObject()->property("version").toString());
    serviceObject.insert(QStringLiteral("vendor"), haObject()->property("organization").toString());

    // Fill the application-specific service data
    QJsonObject dataObject;

    serviceObject.insert(QStringLiteral("serviceData"), dataObject);

    QJsonDocument document(serviceObject);

    // We use Compact to save space, after all it has to be machine-parsed.
    writeFile(QString::fromLatin1("%1.%2").arg(haObject()->property("applicationId").toString(), QStringLiteral("hemeraservice")), document.toJson(QJsonDocument::Compact));

    oneThingLessToDo();
}

void ApplicationGenerator::writeRootOperationServiceFiles()
{
    QQmlListReference rootOperations(haObject(), "rootOperations", haQmlEngine());

    for (int i = 0; i < rootOperations.count(); ++i) {
        QObject *rootOperation = rootOperations.at(i);

        QString operationId = rootOperation->property("operationId").toString();
        QVariantMap args;
        args.insert(QStringLiteral("target"), QStringLiteral("hemera-rootoperation-%1").arg(operationId));
        QString command;

        for (Hemera::Generators::GeneratorPlugin *plugin : plugins()) {
            command = plugin->handlePayloadForClassName(haObject()->metaObject()->className(), QStringLiteral("root-operation-systemd-exec"), args);
            if (!command.isEmpty()) {
                break;
            }
        }

        QString content = QString::fromLatin1(payload(QDir(Hemera::Literals::literal(Hemera::StaticConfig::hemeraDataDir()))
                                                            .absoluteFilePath(QStringLiteral("hemera_root_operation@.service.in"))))
                        .arg(operationId)                                                           // %1   operationId
                        .arg(command);                                                              // %2   exec
        QString socketContent = QString::fromLatin1(payload(QDir(Hemera::Literals::literal(Hemera::StaticConfig::hemeraDataDir()))
                                                                .absoluteFilePath(QStringLiteral("hemera_root_operation.socket.in"))))
                        .arg(operationId);                                                          // %1   operationId
                        // TODO
                        //.arg(debugCommand)                                                          // %2   user
                        //.arg(execOptions)                                                           // %3   group

        writeFile(QStringLiteral("hemera-rootoperation-%1@.service").arg(operationId), content.toLatin1());
        writeFile(QStringLiteral("hemera-rootoperation-%1.socket").arg(operationId), socketContent.toLatin1());

        // Hemera service
        QJsonObject serviceObject;
        // It's a root operation.
        serviceObject.insert(QStringLiteral("type"), static_cast<int>(Hemera::ServiceInfo::ServiceType::RootOperation));

        // Fill the common data
        serviceObject.insert(QStringLiteral("id"), operationId);
        serviceObject.insert(QStringLiteral("parentApplication"), haObject()->property("applicationId").toString());

        // Fill the application-specific service data
        QJsonObject dataObject;

        serviceObject.insert(QStringLiteral("serviceData"), dataObject);

        QJsonDocument document(serviceObject);

        // We use Compact to save space, after all it has to be machine-parsed.
        writeFile(QString::fromLatin1("hemera-rootoperation-%1.hemeraservice").arg(operationId), document.toJson(QJsonDocument::Compact));
    }
}
