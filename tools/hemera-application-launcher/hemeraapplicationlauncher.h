#ifndef HEMERAAPPLICATIONLAUNCHER_H
#define HEMERAAPPLICATIONLAUNCHER_H

#include <HemeraCore/HeadlessApplication>

#include <QtCore/QProcess>

#include <QtCore/QLoggingCategory>

class QProcess;
class QTimer;

#define MAX_CHAR_CHUNK 4096

class HemeraApplicationLauncher : public Hemera::HeadlessApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(HemeraApplicationLauncher)

public:
    HemeraApplicationLauncher(Hemera::ApplicationProperties *properties, QString executable, QStringList arguments);
    virtual ~HemeraApplicationLauncher();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void startImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void stopImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void prepareForShutdown() Q_DECL_OVERRIDE Q_DECL_FINAL;

private Q_SLOTS:
    void onProcessError(QProcess::ProcessError error);

private:
    inline QByteArray processOutput() {
        QByteArray res = QByteArray();
        res.reserve(m_process->bytesAvailable());

        while (m_process->bytesAvailable() > 0) {
            int bytes = m_process->bytesAvailable() > MAX_CHAR_CHUNK ? MAX_CHAR_CHUNK : m_process->bytesAvailable();

            char *buf = new char[bytes];
            qint64 lineLength = m_process->read(buf, bytes);

            if (lineLength < 0) {
                // Make it printable somehow...
                int lastpos = 0, howmany = 0;
                for (int i = 0; i < lineLength; ++i) {
                    if (!QChar::isPrint(buf[i])) {
                        // append
                        res.append(buf + lastpos, howmany);
                        lastpos = i + 1;
                        howmany = 0;
                    } else {
                        ++howmany;
                    }
                }
            }
            delete buf;
        }

        return res;
    }

    QProcess *m_process;
    QTimer *m_terminateTimeout;
    QString m_executable;
    QStringList m_arguments;
};

Q_DECLARE_LOGGING_CATEGORY(DBG_HEMERA_PROXIED_APPLICATION)

#endif // HEMERAAPPLICATIONLAUNCHER_H
