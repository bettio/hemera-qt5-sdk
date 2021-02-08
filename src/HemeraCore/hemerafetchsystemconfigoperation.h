#ifndef HEMERA_FETCHSYSTEMCONFIGOPERATION_H
#define HEMERA_FETCHSYSTEMCONFIGOPERATION_H

#include <HemeraCore/RootOperationClient>
#include <HemeraCore/Global>

namespace Hemera {

class FetchSystemConfigOperationPrivate;
/**
 * @class FetchSystemConfigOperation
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemerasystemconfig.h <HemeraCore/SystemConfig>
 *
 * @brief Fetches a parameter from Hemera's persistent system configuration.
 *
 * Every Hemera appliance has a fixed, persistent and private system configuration which
 * can be used to read core parameters. Some predefined variables defining the global
 * system behavior are there, and some appliance-specific variables can be used to
 * control the global behavior.
 *
 * This Operation allows the Application, if authorized, to retrieve the value of a key in this
 * configuration area.
 *
 * @sa SetSystemConfigOperation
 */
class HEMERA_QT5_SDK_EXPORT FetchSystemConfigOperation : public RootOperationClient
{
    Q_OBJECT
    Q_DISABLE_COPY(FetchSystemConfigOperation)
    Q_DECLARE_PRIVATE(FetchSystemConfigOperation)

public:
    explicit FetchSystemConfigOperation(const QString &key, QObject *parent = nullptr);
    explicit FetchSystemConfigOperation(const QString &key, ExecutionOptions options, QObject *parent = nullptr);
    virtual ~FetchSystemConfigOperation();

    QString value() const;
};

}

#endif // HEMERA_FETCHSYSTEMCONFIGOPERATION_H
