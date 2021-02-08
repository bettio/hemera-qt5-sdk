#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

#include <HemeraCore/AsyncInitObject>

class DebugHelper : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DebugHelper)

public:
    explicit DebugHelper(const QString &application, const QString &orbit, const QString &mode, const QString &interface, bool holder, QObject* parent = 0);
    virtual ~DebugHelper();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

private Q_SLOTS:
    void tryFindingPidForApplication();
    void startDebugging();

private:
    void findPidForApplication();

    QString m_application;
    QString m_orbit;
    QString m_mode;
    QString m_interface;
    bool m_holder;

    int m_triesLeft;
    QString m_executable;
    int m_pid;
    QString m_username;
    int m_uid;
};

#endif // DEBUGHELPER_H
