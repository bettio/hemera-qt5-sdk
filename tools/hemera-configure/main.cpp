#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include "configure.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName(Configure::tr("Hemera SimpleApplication Generator"));
    app.setOrganizationDomain(QStringLiteral("com.ispirata.Hemera"));
    app.setOrganizationName(QStringLiteral("Ispirata"));

    QStringList arguments = app.arguments();

    // Usage: hemera-configure my.app.ha + myapp.pro (OPTIONAL)
    if (arguments.count() < 2) {
        return 0;
    }

    Configure *ag = nullptr;

    if (arguments.count() == 2) {                                               // just the .ha file
        ag = new Configure(arguments.at(1));
    } else if (arguments.count() == 3) {                                        // .ha + dest dir
        // this case is only for our compile tools as we need to specify where
        // to deploy the cmakelists.txt file on our vm during compilation
        ag = new Configure(arguments.at(1), arguments.at(2));
    } else {
        return 0;
    }

    Hemera::Operation *op = ag->init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << Configure::tr("Configure could not be initialized. %1: %2").arg(op->errorName(), op->errorMessage()).toStdString() << std::endl;
            QCoreApplication::instance()->exit(1);
        }
    });

    int exitCode = app.exec();
    delete ag;

    return exitCode;
}
