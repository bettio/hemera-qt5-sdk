#ifndef HEMERA_SETSYSTEMCONFIGOPERATION_H
#define HEMERA_SETSYSTEMCONFIGOPERATION_H

#include <HemeraCore/RootOperationClient>
#include <HemeraCore/Global>

namespace Hemera {

class SetSystemConfigOperationPrivate;
/**
 * @class SetSystemConfigOperation
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemerasystemconfig.h <HemeraCore/SystemConfig>
 *
 * @brief Sets a parameter in Hemera's persistent system configuration.
 *
 * Every Hemera appliance has a fixed, persistent and private system configuration which
 * can be used to read core parameters. Some predefined variables defining the global
 * system behavior are there, and some appliance-specific variables can be used to
 * control the global behavior.
 *
 * This Operation allows the Application, if authorized, to set an option in this
 * configuration area.
 *
 * @note There is a hard limitation on which variables can be written, and the API
 *       (intentionally) offers no way to know which keys are writable. The Application
 *       should know in advance what to do if it needs to use this feature.
 *
 * @sa SetSystemConfigOperation
 */
class HEMERA_QT5_SDK_EXPORT SetSystemConfigOperation : public RootOperationClient
{
    Q_OBJECT
    Q_DISABLE_COPY(SetSystemConfigOperation)
    Q_DECLARE_PRIVATE(SetSystemConfigOperation)

public:
    explicit SetSystemConfigOperation(const QString &key, const QString &value, QObject *parent = nullptr);
    explicit SetSystemConfigOperation(const QString &key, const QString &value, ExecutionOptions options, QObject *parent = nullptr);
    virtual ~SetSystemConfigOperation();
};

}

#endif // HEMERA_SETSYSTEMCONFIGOPERATION_H

