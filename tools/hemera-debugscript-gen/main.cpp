#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include "debugscriptgenerator.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName(DebugScriptGenerator::tr("Hemera Debug Script Generator"));
    app.setOrganizationDomain(QStringLiteral("com.ispirata.Hemera"));
    app.setOrganizationName(QStringLiteral("Ispirata"));

    QCommandLineParser parser;
    parser.setApplicationDescription(DebugScriptGenerator::tr("Hemera Debug Script Generator"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(QStringLiteral("application"), DebugScriptGenerator::tr("Application for which the debug script should be generated."));

    QCommandLineOption prefixOption(QStringList() << QStringLiteral("p") << QStringLiteral("prefix"), DebugScriptGenerator::tr("Prefix for debug"), QStringLiteral("prefix"));
    parser.addOption(prefixOption);
    QCommandLineOption suffixOption(QStringList() << QStringLiteral("s") << QStringLiteral("suffix"), DebugScriptGenerator::tr("Suffix for debug"), QStringLiteral("suffix"));
    parser.addOption(suffixOption);

    parser.process(app);

    if (parser.positionalArguments().count() != 1) {
        return EXIT_FAILURE;
    }

    DebugScriptGenerator dsg(parser.positionalArguments().first(), parser.value(prefixOption), parser.value(suffixOption));

    Hemera::Operation *op = dsg.init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << DebugScriptGenerator::tr("Debug Script Generator could not be initialized. %1: %2")
                         .arg(op->errorName(), op->errorMessage()).toStdString() << std::endl;
            QCoreApplication::instance()->exit(EXIT_FAILURE);
        }
    });

    return app.exec();
}
