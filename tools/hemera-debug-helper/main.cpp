#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include "debughelper.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName(DebugHelper::tr("Hemera Debug Helper"));
    app.setOrganizationDomain(QStringLiteral("com.ispirata.Hemera"));
    app.setOrganizationName(QStringLiteral("Ispirata"));

    QCommandLineParser parser;
    parser.setApplicationDescription(DebugHelper::tr("Hemera Debug Helper"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(QStringLiteral("application"), DebugHelper::tr("Application which should be debugged."));

    QCommandLineOption modeOption(QStringList() << QStringLiteral("mode"),
                                  DebugHelper::tr("Specify which debugging mode to use for the application. Available modes: gdb."),
                                  QStringLiteral("debug_mode"), QStringLiteral("gdb"));
    parser.addOption(modeOption);
    QCommandLineOption holderOption(QStringList() << QStringLiteral("release-holder"),
                                    DebugHelper::tr("Do not start the debbugger, just release the holder for the target application."));
    parser.addOption(holderOption);
    QCommandLineOption orbitalOption(QStringList() << QStringLiteral("orbital-application"),
                                     DebugHelper::tr("Debugging an orbital application. Either this or --orbit <orbit> must be set."));
    parser.addOption(orbitalOption);
    QCommandLineOption orbitOption(QStringList() << QStringLiteral("orbit"),
                                   DebugHelper::tr("Specify in which orbit the application to be debugged lives. Either this or --orbital-application must be set."),
                                   QStringLiteral("orbit_name"));
    parser.addOption(orbitOption);

    QCommandLineOption interfaceOption(QStringList() << QStringLiteral("interface") << QStringLiteral("i"),
                                   DebugHelper::tr("Debug interface"),
                                   QStringLiteral("interface"));
    parser.addOption(interfaceOption);


    parser.process(app);

    if ((parser.positionalArguments().count() != 1) && !parser.isSet(interfaceOption)) {
        return EXIT_FAILURE;
    }

    QString orbitName;
    if (parser.isSet(orbitOption)) {
        orbitName = parser.value(orbitOption);
    } else {
        if (parser.positionalArguments().count() > 0) {
            orbitName = parser.positionalArguments().first();
            orbitName.remove(QLatin1Char('.'));
        }
    }

    QString application;
    if (parser.positionalArguments().count() > 0) {
        application = parser.positionalArguments().first();
    }
    DebugHelper dh(application, orbitName, parser.value(modeOption), parser.value(interfaceOption), parser.isSet(holderOption));

    Hemera::Operation *op = dh.init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << DebugHelper::tr("Debug Helper could not be initialized. %1: %2").arg(op->errorName(), op->errorMessage()).toStdString()
                      << std::endl;
            QCoreApplication::instance()->exit(EXIT_FAILURE);
        }
    });

    return app.exec();
}
