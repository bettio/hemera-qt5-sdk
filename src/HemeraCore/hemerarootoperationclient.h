#ifndef HEMERA_ROOTOPERATIONCLIENT_H
#define HEMERA_ROOTOPERATIONCLIENT_H

#include <HemeraCore/NetworkTransparentOperationClient>

namespace Hemera {

class RootOperationClientPrivate;
/**
 * @class RootOperationClient
 * @ingroup HemeraCore
 * @headerfile Hemera/hemerarootoperationclient.h <HemeraCore/RootOperationClient>
 *
 * @brief A client interface for an operation executed by root.
 *
 * Hemera Applications are never run as root for a number of reasons, among which the fact that it
 * would pose a security threat. However, at times it might be necessary to perform some operations as root.
 *
 * RootOperationClient leverages NetworkTransparentOperationClient to execute through a secure root worker
 * built upon @ref RootOperation just a small portion of code as an Operation, tracking and controlling it.
 *
 * @par Implementation
 * RootOperationClient is a standalone class which only needs to know the ID of the RootOperation to initiate
 * a connection and execute it. Just treat RootOperationClient as a standard Operation -
 * protocol and transport details are already implemented under the hood.
 *
 * @sa NetworkTransparentOperationClient
 * @sa RootOperation
 */
class HEMERA_QT5_SDK_EXPORT RootOperationClient : public NetworkTransparentOperationClient
{
    Q_OBJECT
    Q_DISABLE_COPY(RootOperationClient)
    Q_DECLARE_PRIVATE(RootOperationClient)

    Q_PRIVATE_SLOT(d_func(), void initClient())

public:
    /**
     * @brief Constructs a new RootOperationClient.
     *
     * Once the RootOperationClient is created, a connection to the worker will be initiated,
     * and the operation will be started straight away as soon as the connection is set up.
     *
     * @p id The Operation ID.
     * @p parameters A set of parameters to be passed to the worker. This is optional and dependent on
     *               how the RootOperation is implemented.
     * @p option ExecutionOptions for this Operation.
     * @p parent The QObject parent for this Operation.
     */
    explicit RootOperationClient(const QString &id, const QJsonObject &parameters = QJsonObject(),
                                 ExecutionOptions options = NoOptions, QObject *parent = nullptr);
    /**
     * Default destructor
     */
    virtual ~RootOperationClient();

protected:
    explicit RootOperationClient(RootOperationClientPrivate &dd, const QString &id, const QJsonObject &parameters = QJsonObject(),
                                 ExecutionOptions options = NoOptions, QObject *parent = nullptr);
};
}

#endif // HEMERA_ROOTOPERATIONCLIENT_H
