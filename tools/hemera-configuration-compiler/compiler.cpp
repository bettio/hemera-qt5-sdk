#include "compiler.h"

#include <hemeractconfig.h>

#include <iostream>

Compiler::Compiler(const QStringList& arguments, QObject* parent)
    : BaseCompiler(arguments, true, parent)
{
    std::cout << tr("Hemera Configuration Compiler version %1").arg(QLatin1String(Hemera::StaticConfig::qt5SDKVersion())).toStdString() << std::endl;
}

Compiler::~Compiler()
{

}

void Compiler::initImpl()
{
    startCompilation();
    setReady();
}
