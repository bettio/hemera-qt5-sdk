#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include "applicationgenerator.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName(ApplicationGenerator::tr("Hemera Application Generator"));
    app.setOrganizationDomain(QStringLiteral("com.ispirata.Hemera"));
    app.setOrganizationName(QStringLiteral("Ispirata"));

    QStringList arguments = app.arguments();

    // Usage: hemera-application-gen <hafile> <target>
    if (arguments.count() != 3) {
        return 0;
    }

    ApplicationGenerator ag(arguments.at(1), arguments.at(2));

    Hemera::Operation *op = ag.init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << ApplicationGenerator::tr("ApplicationGenerator could not be initialized. %1: %2")
                         .arg(op->errorName(), op->errorMessage()).toStdString() << std::endl;
            QCoreApplication::instance()->exit(1);
        }
    });

    return app.exec();
}
