#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <HemeraCore/Operation>

#include "compiler.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName(Compiler::tr("Hemera Configuration Compiler"));
    app.setOrganizationDomain(QStringLiteral("com.ispirata.Hemera"));
    app.setOrganizationName(QStringLiteral("Ispirata"));

    QStringList arguments = app.arguments();

    // Usage: hemera-configure my.app.ha + myapp.pro (OPTIONAL)
    if (arguments.count() < 2) {
        return 0;
    }

    Compiler cmp(QCoreApplication::arguments());

    Hemera::Operation *op = cmp.init();

    QObject::connect(op, &Hemera::Operation::finished, [op] {
        if (op->isError()) {
            std::cerr << Compiler::tr("Base Compiler could not be initialized. %1: %2")
                         .arg(op->errorName(), op->errorMessage()).toStdString() << std::endl;
            QCoreApplication::instance()->exit(1);
        }
    });

    return app.exec();
}
