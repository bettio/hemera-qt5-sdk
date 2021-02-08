#ifndef DEBUGSCRIPTGENERATOR_H
#define DEBUGSCRIPTGENERATOR_H

#include <HemeraCore/AsyncInitObject>

class DebugScriptGenerator : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DebugScriptGenerator)

public:
    explicit DebugScriptGenerator(const QString &application, const QString &prefix, const QString &suffix, QObject* parent = nullptr);
    virtual ~DebugScriptGenerator();

public Q_SLOTS:
    void writeDebugScript();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    QString m_application;
    QString m_prefix;
    QString m_suffix;
};

#endif // DEBUGSCRIPTGENERATOR_H
