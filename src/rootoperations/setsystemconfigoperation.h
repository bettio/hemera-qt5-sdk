#ifndef SETSYSTEMCONFIGOPERATION_H
#define SETSYSTEMCONFIGOPERATION_H

#include <HemeraCore/RootOperation>

class QProcess;

class SetSystemConfigOperation : public Hemera::RootOperation
{
    Q_OBJECT
public:
    explicit SetSystemConfigOperation(const QString &id, QObject *parent = nullptr);

protected:
    virtual void startImpl() override;

private Q_SLOTS:
    void onProcessFinished(int exitCode);

private:
    QProcess *m_process;
};

#endif
