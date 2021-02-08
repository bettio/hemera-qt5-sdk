#ifndef HEMERA_HEMERAAPPLICATION_P_H
#define HEMERA_HEMERAAPPLICATION_P_H

#include <HemeraCore/Application>
#include <private/HemeraCore/hemeraasyncinitdbusobject_p.h>

#include <QtCore/QStringList>

#include <QtDBus/QDBusMessage>

namespace Hemera
{

class ApplicationPrivate : public AsyncInitDBusObjectPrivate
{
public:
    ApplicationPrivate(AsyncInitObject* q) : AsyncInitDBusObjectPrivate(q), status(Application::ApplicationStatus::NotInitialized)
                                           , undergoneFailure(false), isSatellite(false), mcPid(0) {}
    virtual ~ApplicationPrivate() {}

    Q_DECLARE_PUBLIC(Application)

    ApplicationProperties *properties;
    Application::ApplicationStatus status;
    QTranslator *qt5SDKTranslator;

    Gravity *gravity;

    QDBusMessage requestStartMessage;
    QDBusMessage requestStopMessage;
    QDBusMessage requestQuitMessage;

    bool undergoneFailure;
    bool isSatellite;

    quint64 mcPid;
    QStringList allowedBusAddresses;
    QString orbitDir;

    QString errorName;
    QString errorMessage;

    QHash< QString, QString > resourceVariables;

    virtual void initHook() Q_DECL_OVERRIDE;
    void setStatus(Application::ApplicationStatus newStatus);
    void quitImpl();
    bool isInAllowedCached(const QString &busAddr);
    QString replaceResourceVariables(const QString &path);

    virtual void postInitHook();

    // Unix signal handlers.
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
};

}

#endif
