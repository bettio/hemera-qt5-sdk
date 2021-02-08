#include "hemeraapplication_p.h"
#include "hemeragravity_p.h"
#include "hemeradbusapplicationholder_p.h"
#include "hemeracommonoperations.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtCore/QCommandLineParser>

#include <QtDBus/QDBusPendingReply>

#include "hemeractconfig.h"
#include <HemeraCore/ApplicationProperties>
#include <HemeraCore/CommonOperations>
#include <HemeraCore/Literals>
#include <HemeraCore/Gravity>

#include <pwd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <systemd/sd-daemon.h>
#include <systemd/sd-login.h>
#include <unistd.h>

#include "applicationadaptor.h"
#include "dbusobjectadaptor.h"
#include "parsecinterface.h"

Q_LOGGING_CATEGORY(LOG_HEMERA_APPLICATION, "Hemera::Application")

#define CHECK_DBUS_CALLER(svc) if (!calledFromDBus()) { \
                                qCWarning(LOG_HEMERA_APPLICATION) << "This function cannot be invoked directly - it should only be invoked as " \
                                              "a DBus call. Doing nothing."; \
                                return; \
                            } \
                            if (message().service() != svc && !d->isInAllowedCached(message().service())) { \
                                qCWarning(LOG_HEMERA_APPLICATION) << "Attempting to invoke this function from " << message().service() \
                                           << ". Something is trying to hijack the application. Doing nothing."; \
                                sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::notAllowed()), \
                                               message().service() + QStringLiteral(" is not allowed to perform this call.")); \
                                return; \
                            }
#define CHECK_NOT_DBUS_CALLER if (calledFromDBus()) { \
                                qCWarning(LOG_HEMERA_APPLICATION) << "This function cannot be invoked from the bus. Doing nothing."; \
                                sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::notAllowed()), \
                                               QStringLiteral("This function cannot be invoked from the bus.")); \
                                return; \
                            }
#define CHECK_NOT_DBUS_CALLER_RET(ret) if (calledFromDBus()) { \
                                qCWarning(LOG_HEMERA_APPLICATION) << "This function cannot be invoked from the bus. Doing nothing."; \
                                sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::notAllowed()), \
                                               QStringLiteral("This function cannot be invoked from the bus.")); \
                                return ret; \
                            }

namespace Hemera
{

bool ApplicationPrivate::isInAllowedCached(const QString &busAddr)
{
    // This function has effect *only* on bus addresses
    if (!busAddr.startsWith(QLatin1Char(':'))) {
        return false;
    }

    if (allowedBusAddresses.contains(busAddr)) {
        return true;
    } else {
        // Check the pid
        if (mcPid == 0) {
            // No option but blocking the bus... it should happen only once anyway.
            mcPid = QDBusConnection(Literals::DBus::starBus()).interface()->servicePid(Literals::literal(Literals::DBus::parsecService())).value();
        }

        if (mcPid == QDBusConnection(Literals::DBus::starBus()).interface()->servicePid(busAddr)) {
            allowedBusAddresses.append(busAddr);
            return true;
        }
    }

    return false;
}

void ApplicationPrivate::initHook()
{
    Q_Q(Application);

    setStatus(Application::ApplicationStatus::Initializing);

    // Once ready for shutdown, we can quit. Shall we fail, gracefully quit.
    QObject::connect(q, &Application::applicationStatusChanged, [this] (Hemera::Application::ApplicationStatus status) {
            if (status == Application::ApplicationStatus::ReadyForShutdown) {
            } else if (status == Application::ApplicationStatus::Failed) {
                // Something really bad happened. Force quit.
                undergoneFailure = true;
                quitImpl();
            }
    });

    // Should initialization fail, we should fail totally. Also set the status of this application's service once it's ready.
    QObject::connect(initOperation.data(), &Hemera::Operation::finished, [this, q] (Hemera::Operation *op) {
            if (op->isError()) {
                qCWarning(LOG_HEMERA_APPLICATION) << Application::tr("Init error occurred!") << op->errorName() << op->errorMessage();
                errorName = op->errorName();
                errorMessage = op->errorMessage();
                setStatus(Application::ApplicationStatus::Failed);
            } else {
                // Change the status
                setStatus(Application::ApplicationStatus::Stopped);
                // Notify systemd the application has started
                sd_notify(0, "READY=1");
            }
    });

    if (Q_UNLIKELY(!QCoreApplication::instance())) {
        q->setInitError(Literals::literal(Literals::Errors::badRequest()),
                        Application::tr("Hemera::Application has been instantiated without a QCoreApplication. This should never happen."));
        return;
    }

    // Inject properties into QCoreApplication
    QCoreApplication::instance()->setApplicationName(properties->name());
    QCoreApplication::instance()->setApplicationVersion(properties->version());
    QCoreApplication::instance()->setOrganizationDomain([this] () -> QString {
        QStringList fields = properties->id().split(QLatin1Char('.'));
        QString result;
        for (int i = 0; i < fields.count() - 1; ++i) {
            result.append(fields.at(i));
            result.append(QLatin1Char('.'));
        }
        result.chop(1);
        return result;
    }());
    QCoreApplication::instance()->setOrganizationName(properties->organization());

    // Ok, now... We need our star bus ready and kicking as well. Open the connection!
    QDBusConnection starBusConnection = QDBusConnection::connectToBus(QStringLiteral("unix:path=%1/%2/dbus/star_bus_socket")
                                                                        .arg(QLatin1String(StaticConfig::hemeraStarsRuntimeDir()),
                                                                             QLatin1String(qgetenv("HEMERA_STAR"))), Literals::DBus::starBus());
    if (!starBusConnection.isConnected()) {
        q->setInitError(Literals::literal(Literals::Errors::dbusObjectNotAvailable()),
                        Application::tr("Couldn't open a connection to the Star bus. This is a serious internal error."));
        return;
    }

    if (!QDBusConnection::sessionBus().registerService(q->id())) {
        q->setInitError(Literals::literal(Literals::Errors::registerServiceFailed()),
                        Application::tr("Couldn't register the bus service. This means a problem in the installation of the app."));
        return;
    }

    if (!QDBusConnection::sessionBus().registerObject(Literals::literal(Literals::DBus::applicationPath()), q)) {
        q->setInitError(Literals::literal(Literals::Errors::registerObjectFailed()),
                        Application::tr("Couldn't register the bus object. This is a severe runtime error, your Orbit might be compromised."));
        return;
    }

    if (!starBusConnection.registerService(q->id())) {
        q->setInitError(Literals::literal(Literals::Errors::registerServiceFailed()),
                        Application::tr("Couldn't register the bus service. This means a problem in the installation of the app."));
        return;
    }

    if (!starBusConnection.registerObject(Literals::literal(Literals::DBus::applicationPath()), q)) {
        q->setInitError(Literals::literal(Literals::Errors::registerObjectFailed()),
                        Application::tr("Couldn't register the bus object. This is a severe runtime error, your Orbit might be compromised."));
        return;
    }

    // Allow the signal to spread
    QCoreApplication::processEvents();

    // Setup systemd's software watchdog, if enabled
    bool watchdogConverted;
    qulonglong watchdogTime = qgetenv("WATCHDOG_USEC").toULongLong(&watchdogConverted);
    if (watchdogConverted && watchdogTime > 0) {
        qCDebug(LOG_HEMERA_APPLICATION) << "Setting up systemd watchdog";
        QTimer *watchdogTimer = new QTimer(q);
        watchdogTimer->setSingleShot(false);
        watchdogTimer->setInterval(watchdogTime / 1000 / 2);
        QObject::connect(watchdogTimer, &QTimer::timeout, [] {
            sd_notify(0, "WATCHDOG=1");
        });
        watchdogTimer->start();
    }

    qAddPostRoutine([] () -> void {
        if (Application::instance() == nullptr) {
            qCDebug(LOG_HEMERA_APPLICATION) << Application::tr("Application already destroyed");
            return;
        }

        if (Application::instance()->applicationStatus() == Application::ApplicationStatus::ReadyForShutdown ||
            Application::instance()->applicationStatus() == Application::ApplicationStatus::Failed) {
            // We're ok
        } else {
            qCWarning(LOG_HEMERA_APPLICATION) << Application::tr("The application is going for a dirty shutdown!");
        }
    });

    new ApplicationAdaptor(q);

    gravity = new Gravity(q);
    Hemera::Operation *initMh = gravity->init();

    QObject::connect(initMh, &Hemera::Operation::finished, [this, q] (Hemera::Operation *op) {
            if (op->isError()) {
                // Should gravityCenter fail, we should fail totally
                q->setInitError(op->errorName(), op->errorMessage());
            } else {
                // Ok. If our device is in development mode, we want to override category filters to have full debug output from
                // Qt5 SDK and its users. Obviously this doesn't override any custom filter installed by the user.
                if (gravity->isDeveloperModeActive()) {
                    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=true"));
                }

                // Good. If we are here, it means our startup was not compromised by any means. We can now start considering
                // the kind of hooks we want to undertake.
                // To do this, let's consider what kind of arguments the application has.
                QCommandLineParser parser;

                QCommandLineOption attachOption(QStringLiteral("hold-start"));
                parser.addOption(attachOption);

                parser.process(QCoreApplication::arguments());

                if (parser.isSet(attachOption)) {
                    if (!gravity->isDeveloperModeActive()) {
                        // Nice try, buddies. Just no.
                        q->setInitError(Literals::literal(Literals::Errors::badRequest()),
                                        Application::tr("Tried to force the application into starting on hold, but this device reports it's in production mode. "
                                                       "This is a severe runtime error, your Orbit or Device might be compromised."));
                        return;
                    }

                    // Ok, we should be safe in assuming the app is ok. Let's bring up some DBus in this thing.
                    DBusApplicationHolder *holder = new DBusApplicationHolder(q->id(), q);

                    // Get ready for release
                    QObject::connect(holder, &DBusApplicationHolder::released, [this] {
                        // Move forward
                        qCDebug(LOG_HEMERA_APPLICATION) << Application::tr("Hemera Application successfully released.");
                        Hemera::AsyncInitDBusObjectPrivate::initHook();
                    });

                    // Trigger init
                    QObject::connect(holder->init(), &Hemera::Operation::finished, [this, q] (Hemera::Operation *hop) {
                        if (hop->isError()) {
                            // Should the holder fail, we should fail totally
                            q->setInitError(hop->errorName(), hop->errorMessage());
                        } else {
                            // Good. let's just wait for release, and notify.
                            qCDebug(LOG_HEMERA_APPLICATION) << Application::tr("Hemera Application on hold, now waiting for release.");
                        }
                    });

                    // That's all, folks. We'll be back once release is complete.
                    return;
                }

                // Record whether we're a satellite or not.
                DBusBoolOperation *satelliteOp = new DBusBoolOperation(gravity->d->parsec->AmIASatellite());
                QObject::connect(satelliteOp, &Hemera::Operation::finished, [this, satelliteOp] {
                    if (satelliteOp->isError()) {
                        qCWarning(LOG_HEMERA_APPLICATION) << Application::tr("Could not determine whether Application is a Satellite or not! Going on, but this is really wrong!");
                    } else {
                        isSatellite = satelliteOp->result();
                    }

                    Hemera::AsyncInitDBusObjectPrivate::initHook();
                });
            }
    });
}

void ApplicationPrivate::postInitHook()
{
    qCDebug(LOG_HEMERA_APPLICATION) << Application::tr("Registering application!!");
    gravity->d->parsec->registerApplication();

    AsyncInitDBusObjectPrivate::postInitHook();
}

void ApplicationPrivate::setStatus(Application::ApplicationStatus newStatus)
{
    Q_Q(Application);

    status = newStatus;
    Q_EMIT q->applicationStatusChanged(status);

    // Change systemd status
    switch (status) {
        case Application::ApplicationStatus::NotInitialized:
            sd_notify(0, "STATUS=Hemera::Application not initialized - idle");
            break;
        case Application::ApplicationStatus::Initializing:
            sd_notify(0, "STATUS=Hemera::Application is initializing...");
            break;
        case Application::ApplicationStatus::Stopped:
            sd_notify(0, "STATUS=Hemera::Application stopped and idle");
            break;
        case Application::ApplicationStatus::Starting:
            sd_notify(0, "STATUS=Hemera::Application is starting...");
            break;
        case Application::ApplicationStatus::Running:
            sd_notify(0, "STATUS=Hemera::Application running");
            break;
        case Application::ApplicationStatus::Stopping:
            sd_notify(0, "STATUS=Hemera::Application is stopping...");
            break;
        case Application::ApplicationStatus::Failed:
            sd_notifyf(0, "STATUS=Hemera::Application failed: %s\n"
                           "BUSERROR=%s",
                           qstrdup(errorMessage.toLatin1().constData()),
                           qstrdup(errorName.toLatin1().constData()));
            break;
        case Application::ApplicationStatus::ShuttingDown:
            sd_notify(0, "STATUS=Hemera::Application is shutting down...");
            break;
        case Application::ApplicationStatus::ReadyForShutdown:
            if (errorName.isEmpty()) {
                sd_notifyf(0, "STATUS=Hemera::Application is ready for a clean shutdown");
            } else if (!q->isReady()) {
                sd_notifyf(0, "STATUS=Hemera::Application could not be initialized: %s\n"
                           "BUSERROR=%s",
                           qstrdup(errorMessage.toLatin1().constData()),
                           qstrdup(errorName.toLatin1().constData()));
#ifdef ENABLE_TEST_CODEPATHS
                if (Q_UNLIKELY(qgetenv("RUNNING_AUTOTESTS").toInt() != 1)) {
                    QCoreApplication::instance()->exit(1);
                }
#endif
            } else {
                sd_notifyf(0, "STATUS=Hemera::Application is ready for a clean shutdown, but previously failed with: %s\n"
                           "BUSERROR=%s",
                           qstrdup(errorMessage.toLatin1().constData()),
                           qstrdup(errorName.toLatin1().constData()));
            }
            break;
        case Application::ApplicationStatus::Unknown:
            sd_notify(0, "STATUS=Hemera::Application is in an unknown status");
            break;
    }
}

QString ApplicationPrivate::replaceResourceVariables(const QString &path)
{
    QString subPath = path;
    for (QHash< QString, QString >::const_iterator i = resourceVariables.constBegin(); i != resourceVariables.constEnd(); ++i) {
        subPath.replace(QStringLiteral("$%1").arg(i.key()), i.value());
    }

    return subPath;
}

class ApplicationHelper
{
public:
    ApplicationHelper() : q(nullptr) {}
    ~ApplicationHelper() {
        // Do not delete - the object will be cleaned up already.
    }
    Application *q;
};

Q_GLOBAL_STATIC(ApplicationHelper, s_globalApplication)

Application *Application::instance()
{
    if (!s_globalApplication()->q) {
        return nullptr;
    }

    return s_globalApplication()->q;
}


Application::Application(Hemera::ApplicationPrivate& dd, Hemera::ApplicationProperties* properties)
    : AsyncInitDBusObject(dd, nullptr)
{
    Q_ASSERT(!s_globalApplication()->q);
    s_globalApplication()->q = this;
    connect(this, &QObject::destroyed, [] {
        s_globalApplication()->q = nullptr;
    });

    Q_D(Application);
    d->properties = properties;

    // Load translations
    d->qt5SDKTranslator = new QTranslator(this);
    if (d->qt5SDKTranslator->load(QLocale(), QStringLiteral("qt5sdk"), QStringLiteral("_"), QLatin1String(StaticConfig::translationsDir()))) {
        QCoreApplication::installTranslator(d->qt5SDKTranslator);
    }

    // To allow everyone to benefit from Application's features, we need to retrieve the orbit name here.
    // Sorry, buddies.
    uid_t uid;
    sd_pid_get_owner_uid(QCoreApplication::applicationPid(), &uid);
    // Get the user. Do NOT free user_data!! Unix handles that memory.
    passwd *user_data = getpwuid(uid);
#ifdef ENABLE_TEST_CODEPATHS
    if (Q_LIKELY(user_data)) {
#endif
        d->orbitDir = QString::fromLatin1(qstrdup(user_data->pw_dir));
#ifdef ENABLE_TEST_CODEPATHS
    } else {
        qCWarning(LOG_HEMERA_APPLICATION) << tr("Could not retrieve user data! This should never happen, unless you're running an autotest.");
    }

    if (Q_UNLIKELY(qgetenv("RUNNING_AUTOTESTS").toInt() == 1)) {
        qCDebug(LOG_HEMERA_APPLICATION) << tr("Apparently you are running an autotest. Will set orbit dir to %1 for your ease.").arg(QDir::currentPath());
        d->orbitDir = QDir::currentPath();
    }
#endif

    // Release memory
    endpwent();
}

Application::~Application()
{
    Q_D(Application);
    delete d->properties;
}

Application::ApplicationStatus Application::applicationStatus() const
{
    Q_D(const Application);

    return d->status;
}

QString Application::id()
{
    return Application::instance()->d_func()->properties->id();
}

bool Application::isSatellite()
{
    return Application::instance()->d_func()->isSatellite;
}

QString Application::resourcePath(const QString &p, Application::ResourceDomain domain, bool checkPathExistence)
{
    QString path = Application::instance()->d_func()->replaceResourceVariables(p);
    QString resourcePath = QStringLiteral("%1%2%3%2%4%2%5").arg(Application::instance()->d_func()->orbitDir, QDir::separator(),
                                                                QStringLiteral(".config"), id(), path);
    if ((QFile::exists(resourcePath) || !checkPathExistence) && domain >= ResourceDomain::Orbit) {
        // Orbit domain
        return QDir::cleanPath(resourcePath);
    }

    resourcePath = QStringLiteral("%1%2%3%2%4").arg(QLatin1String(Hemera::StaticConfig::haOwnResourceDir()), QDir::separator(),
                                                         id(), path);
    if ((QFile::exists(resourcePath) || !checkPathExistence) && domain >= ResourceDomain::Application) {
        // Application domain
        return QDir::cleanPath(resourcePath);
    }

    resourcePath = QStringLiteral("%1%2%3%2%4").arg(QLatin1String(Hemera::StaticConfig::haBaseResourceDir()), QDir::separator(),
                                                         id(), path);
    if (QFile::exists(resourcePath) || !checkPathExistence) {
        // Defaults domain
        return QDir::cleanPath(resourcePath);
    }

    return QString();
}

qint64 Application::writeToResource(const QString& path, QIODevice* data, Application::ResourceDomain domain)
{
    return writeToResource(path, data->readAll(), domain);
}

qint64 Application::writeToResource(const QString& path, const QByteArray& data, Application::ResourceDomain domain)
{
    return writeToResource(path, data.constData(), data.size(), domain);
}

qint64 Application::writeToResource(const QString& p, const char* data, qint64 size, Application::ResourceDomain domain)
{
    QString filePath;
    QString path = resourcePath(p, domain, false);

    // Make sure the directory exists
    QDir dir = QFileInfo(path).dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return -1;
        }
    }

    QFile f(path);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return -1;
    }

    qint64 ret = f.write(data, size);
    f.flush();
    f.close();

    return ret;
}

void Application::setResourceVariable(const QString &variable, const QString &value)
{
    Application::instance()->d_func()->resourceVariables.insert(variable, value);
}

Gravity *Application::gravity() const
{
    Q_D(const Application);
    return d->gravity;
}

int Application::exec()
{
    CHECK_NOT_DBUS_CALLER_RET(1)

    // There, we execute the main loop. Before this, trigger a delayed init procedure right after.
    init();
    return QCoreApplication::instance()->exec();
}

void ApplicationPrivate::quitImpl()
{
    Q_Q(Application);

    if (status == Application::ShuttingDown) {
        // This can only happen on a direct quitImpl call
        qCWarning(LOG_HEMERA_APPLICATION) << Application::tr("quitImpl invoked, but the application is already in ShuttingDownStatus. Aborting.");
        return;
    }

    auto beginShutdownProcedure = [this, q] () {
        setStatus(Application::ApplicationStatus::ShuttingDown);
        QTimer::singleShot(0, q, SLOT(prepareForShutdown()));
    };

    if (status == Application::ApplicationStatus::Stopped || status == Application::ApplicationStatus::NotInitialized ||
        status == Application::ApplicationStatus::Unknown || status == Application::ApplicationStatus::Failed) {
        // Then we can safely start the shutdown procedure. In case the state is unknown, we just quit as there's not much we can do.
        // If the application is failed, we simply clean up.
        beginShutdownProcedure();
    } else {
        QObject::connect(q, &Application::applicationStatusChanged, [this, beginShutdownProcedure] (Hemera::Application::ApplicationStatus status) {
                if (status == Application::ApplicationStatus::Stopped || status == Application::ApplicationStatus::Failed) {
                    // Begin shutdown procedure
                    beginShutdownProcedure();
                }
        });

        auto triggerStop = [this, q] (Hemera::Application::ApplicationStatus status) {
            if (status == Application::Running) {
                // Trigger stop manually
                setStatus(Application::Stopping);
                q->stopImpl();
            }
        };

        if (status == Application::Starting) {
            // Once ready, we can start the stopping procedure.
            QObject::connect(q, &Application::applicationStatusChanged, triggerStop);
        } else if (status == Application::Running) {
            triggerStop(Application::Running);
        }
    }
}

void Application::quit()
{
    Q_D(Application);

    CHECK_DBUS_CALLER(Literals::literal(Literals::DBus::parsecService()))

    if (d->status == ApplicationStatus::ShuttingDown || d->status == ApplicationStatus::Failed || d->status == ApplicationStatus::ReadyForShutdown ||
        d->status == ApplicationStatus::NotInitialized || d->status == ApplicationStatus::Initializing) {
        sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()),
                       tr("The application cannot quit at this time."));
        return;
    }

    // Reply delayed.
    setDelayedReply(true);
    d->requestQuitMessage = message();

    QTimer::singleShot(0, this, SLOT(quitImpl()));
}

void Application::start()
{
    Q_D(Application);

    CHECK_DBUS_CALLER(Literals::literal(Literals::DBus::parsecService()))

    if (d->status != Stopped) {
        sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()),
                       tr("The application is not in StoppedStatus, hence it cannot be started."));
        return;
    }

    // Reply delayed.
    setDelayedReply(true);
    d->requestStartMessage = message();

    d->setStatus(ApplicationStatus::Starting);
    QTimer::singleShot(0, this, SLOT(startImpl()));
}

void Application::setStarted(Hemera::Application::OperationResult result, const QString &errorName, const QString &errorMessage)
{
    Q_D(Application);

    if (d->requestStartMessage.type() == QDBusMessage::InvalidMessage) {
        qCWarning(LOG_HEMERA_APPLICATION) << tr("setStarted was called without a valid pending DBus context!! Something's totally wrong.");
        return;
    }

    if (result == OperationResult::Failed) {
        // Rollback the status to the previous one - in this case, stopped.
        QDBusConnection(Literals::DBus::starBus()).send(d->requestStartMessage.createErrorReply(errorName, errorMessage));
        d->setStatus(ApplicationStatus::Stopped);
    } else if (result == OperationResult::Fatal) {
        // Fatal. Die here.
        QDBusConnection(Literals::DBus::starBus()).send(d->requestStartMessage.createErrorReply(errorName, errorMessage));
        d->errorName = errorName;
        d->errorMessage = errorMessage;
        d->setStatus(ApplicationStatus::Failed);
    } else if (d->status != Starting) {
        qCWarning(LOG_HEMERA_APPLICATION) << tr("setStarted invoked, but the application is not in StartingStatus. Aborting.");
        return;
    } else {
        QDBusConnection(Literals::DBus::starBus()).send(d->requestStartMessage.createReply());
        d->setStatus(ApplicationStatus::Running);
    }

    d->requestStartMessage = QDBusMessage();
}

void Application::stop()
{
    Q_D(Application);

    CHECK_DBUS_CALLER(Literals::literal(Literals::DBus::parsecService()))

    if (d->status != Running) {
        sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()),
                       tr("The application is not in RunningStatus, hence it cannot be started."));
        return;
    }

    // Reply delayed.
    setDelayedReply(true);
    d->requestStopMessage = message();

    d->setStatus(ApplicationStatus::Stopping);
    QTimer::singleShot(0, this, SLOT(stopImpl()));
}

void Application::openURL(const QUrl& url)
{
    Q_D(Application);

    CHECK_DBUS_CALLER(Literals::literal(Literals::DBus::parsecService()))

    setDelayedReply(true);
    QDBusMessage requestMessage = message();
    connect(openURLImpl(url), &Operation::finished, [this, requestMessage] (Operation *op) {
        if (op->isError()) {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createErrorReply(op->errorName(), op->errorMessage()));
        } else {
            QDBusConnection(Literals::DBus::starBus()).send(requestMessage.createReply());
        }
    });
}

void Application::setStopped(Hemera::Application::OperationResult result, const QString &errorName, const QString &errorMessage)
{
    Q_D(Application);

    bool busNotify = true;

    if (d->requestStopMessage.type() == QDBusMessage::InvalidMessage) {
        if (d->requestQuitMessage.type() != QDBusMessage::InvalidMessage) {
            qCDebug(LOG_HEMERA_APPLICATION) << tr("Notice - the application will be stopped before being shutdown");
            busNotify = false;
        } else {
            qCWarning(LOG_HEMERA_APPLICATION) << tr("setStopped was called without a valid pending DBus context!! Something's totally wrong.");
            return;
        }
    }

    if (result == OperationResult::Failed) {
        // Rollback the status to the previous one - in this case, running.
        QDBusConnection(Literals::DBus::starBus()).send(d->requestStopMessage.createErrorReply(errorName, errorMessage));
        d->setStatus(ApplicationStatus::Running);
    } else if (result == OperationResult::Fatal) {
        // Fatal. Die here.
        QDBusConnection(Literals::DBus::starBus()).send(d->requestStopMessage.createErrorReply(errorName, errorMessage));
        d->errorName = errorName;
        d->errorMessage = errorMessage;
        d->setStatus(ApplicationStatus::Failed);
    } else if (d->status != Stopping) {
        qCWarning(LOG_HEMERA_APPLICATION) << tr("setStopped invoked, but the application is not in StartingStatus. Aborting.");
        return;
    } else {
        if (busNotify) {
            QDBusConnection(Literals::DBus::starBus()).send(d->requestStopMessage.createReply());
        }
        d->setStatus(ApplicationStatus::Stopped);
    }

    d->requestStopMessage = QDBusMessage();
}

void Application::setReadyForShutdown(Hemera::Application::OperationResult result, const QString &errorName, const QString &errorMessage)
{
    Q_D(Application);

    if (d->requestQuitMessage.type() == QDBusMessage::InvalidMessage && !d->undergoneFailure) {
        qCWarning(LOG_HEMERA_APPLICATION) << tr("setReadyForShutdown was called without a valid pending DBus context!! Something's totally wrong.");
        return;
    }

    if (result == OperationResult::Failed) {
        // Rollback the status to the previous one - in this case, stopped.
        if (!d->undergoneFailure) {
            QDBusConnection(Literals::DBus::starBus()).send(d->requestQuitMessage.createErrorReply(errorName, errorMessage));
            d->setStatus(ApplicationStatus::Stopped);
        } else {
            // There's no point in trying again. Die here.
            d->setStatus(ApplicationStatus::ReadyForShutdown);
        }
        return;
    } else if (result == OperationResult::Fatal) {
        // Fatal. Die here immediately.
        qCWarning(LOG_HEMERA_APPLICATION) << tr("setReadyForShutdown failed! Terminating the application anyway as requested.");
        if (!d->undergoneFailure) {
            QDBusConnection(Literals::DBus::starBus()).send(d->requestQuitMessage.createErrorReply(errorName, errorMessage));
        }
        d->errorName = errorName;
        d->errorMessage = errorMessage;
        d->setStatus(ApplicationStatus::ReadyForShutdown);
        return;
    } else if (d->status != ApplicationStatus::ShuttingDown) {
        qCWarning(LOG_HEMERA_APPLICATION) << tr("setReadyForShutdown invoked, but the application is not in ShuttingDownStatus. Aborting.");
        return;
    } else {
        if (!d->undergoneFailure) {
            QDBusConnection(Literals::DBus::starBus()).send(d->requestQuitMessage.createReply());
        }
        d->setStatus(ApplicationStatus::ReadyForShutdown);
    }

    d->requestQuitMessage = QDBusMessage();
}

void Application::prepareForShutdown()
{
    setReadyForShutdown();
}

Operation* Application::openURLImpl(const QUrl& url)
{
    Q_UNUSED(url);
    return new FailureOperation(Literals::literal(Literals::Errors::badRequest()), tr("The given URL is not handled by this application."));
}

Operation *Application::requestOrbitSwitchInhibition(const QString &reason)
{
    Q_D(Application);

    return d->gravity->d->requestOrbitSwitchInhibition(reason);
}

Operation *Application::releaseOrbitSwitchInhibition()
{
    Q_D(Application);

    return d->gravity->d->releaseOrbitSwitchInhibition();
}

}

#include "moc_hemeraapplication.cpp"
