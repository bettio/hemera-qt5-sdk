#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include "propertiesgenerator.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName(PropertiesGenerator::tr("Hemera Properties Generator"));
    app.setOrganizationDomain(QStringLiteral("com.ispirata.Hemera"));
    app.setOrganizationName(QStringLiteral("Ispirata"));

    QStringList availableSdks = QStringList() << QStringLiteral("qt5") << QStringLiteral("python");

    QCommandLineParser parser;
    parser.setApplicationDescription(PropertiesGenerator::tr("Hemera Properties Generator"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(QStringLiteral("ha"), PropertiesGenerator::tr("Ha file to parse."));

    QCommandLineOption sdkOption(QStringList() << QStringLiteral("sdk"),
                                 PropertiesGenerator::tr("SDK for which the properties should be generated. Available choices: %1.").arg(availableSdks.join(QLatin1Char(','))),
                                 QStringLiteral("sdk"), QStringLiteral("qt5"));
    parser.addOption(sdkOption);
    QCommandLineOption classOption(QStringList() << QStringLiteral("class-name"),
                                   PropertiesGenerator::tr("Class name for generated properties."),
                                   QStringLiteral("class-name"));
    parser.addOption(classOption);
    QCommandLineOption outputOption(QStringList() << QStringLiteral("output-name"),
                                    PropertiesGenerator::tr("Output name for generated file(s)."), QStringLiteral("output-name"));
    parser.addOption(outputOption);

    parser.process(app);

    if (parser.positionalArguments().count() != 1) {
        return EXIT_FAILURE;
    }

    if (parser.value(classOption).isEmpty() || parser.value(outputOption).isEmpty() || !availableSdks.contains(parser.value(sdkOption))) {
        std::cerr << PropertiesGenerator::tr("Error parsing arguments, or missing arguments.").toStdString() << std::endl;
        return EXIT_FAILURE;
    }

    PropertiesGenerator ag(parser.positionalArguments().first(), parser.value(sdkOption), parser.value(classOption), parser.value(outputOption));

    Hemera::Operation *op = ag.init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << PropertiesGenerator::tr("PropertiesGenerator could not be initialized. %1: %2")
                         .arg(op->errorName(), op->errorMessage()).toStdString() << std::endl;
            QCoreApplication::instance()->exit(1);
        }
    });

    return app.exec();
}
