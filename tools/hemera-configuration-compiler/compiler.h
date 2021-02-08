#ifndef COMPILER_H
#define COMPILER_H

#include <HemeraGenerators/BaseCompiler>

class Compiler : public Hemera::Generators::BaseCompiler
{
    Q_OBJECT
    Q_DISABLE_COPY(Compiler)

public:
    explicit Compiler(const QStringList& arguments, QObject* parent = nullptr);
    virtual ~Compiler();

protected:
    virtual void initImpl() override final;
};

#endif // COMPILER_H
