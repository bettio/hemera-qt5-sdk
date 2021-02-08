#ifndef HEMERA_CXX_HEMERAAPPLICATION_H
#define HEMERA_CXX_HEMERAAPPLICATION_H

#include <HemeraCore/Global>

#include <HemeraCxxCore/ApplicationProperties>

class QIODevice;

namespace HemeraCxx {

class ProxyQt5Application;
class ProxyQt5ApplicationPrivate;

class ApplicationProperties;
class Gravity;

typedef void (*_fn_impl)(void*);

class HEMERA_QT5_SDK_EXPORT Application
{
public:
    enum class ApplicationStatus : quint8 {
        NotInitialized = HEMERA_ENUM_Application_ApplicationStatus_NotInitialized,
        Initializing = HEMERA_ENUM_Application_ApplicationStatus_Initializing,
        Stopped = HEMERA_ENUM_Application_ApplicationStatus_NotInitialized,
        Starting = HEMERA_ENUM_Application_ApplicationStatus_Starting,
        Running = HEMERA_ENUM_Application_ApplicationStatus_Running,
        Stopping = HEMERA_ENUM_Application_ApplicationStatus_Stopping,
        Failed = HEMERA_ENUM_Application_ApplicationStatus_Failed,
        ShuttingDown = HEMERA_ENUM_Application_ApplicationStatus_ShuttingDown,
        ReadyForShutdown = HEMERA_ENUM_Application_ApplicationStatus_ReadyForShutdown,
        Unknown = HEMERA_ENUM_Application_ApplicationStatus_Unknown
    };

    enum class ResourceDomain : quint8 {
        Defaults = HEMERA_ENUM_Application_ResourceDomain_Defaults,
        Application = HEMERA_ENUM_Application_ResourceDomain_Application,
        Orbit = HEMERA_ENUM_Application_ResourceDomain_Orbit
    };

    enum class OperationResult : quint8 {
        Success = HEMERA_ENUM_Application_OperationResult_Success,
        Failed = HEMERA_ENUM_Application_OperationResult_Failed,
        Fatal = HEMERA_ENUM_Application_OperationResult_Fatal
    };

    typedef void (*_application_status_changed_cb)(HemeraCxx::Application::ApplicationStatus,void*);

    explicit Application(ApplicationProperties *properties);
    virtual ~Application();

    static Application *applicationInstance();

    ApplicationStatus applicationStatus() const;

    static std::string id();
    static std::string resourcePath(const std::string &path, ResourceDomain domain = ResourceDomain::Orbit, bool checkPathExistence = true);

    static long long writeToResource(const std::string &path, const char *data, ResourceDomain domain = ResourceDomain::Orbit);

    bool requestOrbitSwitchInhibition(const std::string& reason = std::string());
    bool releaseOrbitSwitchInhibition();

//     HemeraCxx::Gravity *gravity() const;

    void setPreInitFunction(_fn_impl init, void *data = nullptr);
    void setPreStartFunction(_fn_impl start, void *data = nullptr);
    void setPreStopFunction(_fn_impl stop, void *data = nullptr);
    void setPreShutdownFunction(_fn_impl shutdown, void *data = nullptr);

    void setInitFunction(_fn_impl init, void *data = nullptr);
    void setStartFunction(_fn_impl start, void *data = nullptr);
    void setStopFunction(_fn_impl stop, void *data = nullptr);
    void setShutdownFunction(_fn_impl shutdown, void *data = nullptr);

    int registerApplicationStatusChangedCallback(_application_status_changed_cb cb, void *data = nullptr);
    void unregisterApplicationStatusChangedCallback(int cbId);

    void setReady(OperationResult result = OperationResult::Success,
                  const std::string &errorName = std::string(), const std::string &errorMessage = std::string());
    void setStarted(OperationResult result = OperationResult::Success,
                    const std::string &errorName = std::string(), const std::string &errorMessage = std::string());
    void setStopped(OperationResult result = OperationResult::Success,
                    const std::string &errorName = std::string(), const std::string &errorMessage = std::string());
    void setReadyForShutdown(OperationResult result = OperationResult::Success,
                             const std::string &errorName = std::string(), const std::string &errorMessage = std::string());

    void init();

protected:
    void initImpl();
    void startImpl();
    void stopImpl();
    void prepareForShutdown();

// Q_SIGNALS:
//     void applicationStatusChanged(Hemera::Application::ApplicationStatus status);
private:
    class Private;
    Private * const d;

    friend class HemeraCxx::ProxyQt5Application;
    friend class HemeraCxx::ProxyQt5ApplicationPrivate;
};

// Wrappers
typedef Application ApplicationWrapper;

}

#endif // HEMERA_CXX_HEMERAAPPLICATION_H
