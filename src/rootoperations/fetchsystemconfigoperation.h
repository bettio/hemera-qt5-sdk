#ifndef FETCHSYSTEMCONFIGOPERATION_H
#define FETCHSYSTEMCONFIGOPERATION_H

#include <HemeraCore/RootOperation>

class QProcess;

class FetchSystemConfigOperation : public Hemera::RootOperation
{
    Q_OBJECT
public:
    explicit FetchSystemConfigOperation(const QString &id, QObject *parent = nullptr);

protected:
    virtual void startImpl() override;

private Q_SLOTS:
    void onProcessFinished(int exitCode);

private:
    QProcess *m_process;
};

#endif
