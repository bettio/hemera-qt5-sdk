#ifndef HEMERA_NETWORKTRANSPARENTOPERATION_H
#define HEMERA_NETWORKTRANSPARENTOPERATION_H

#include <HemeraCore/OperationWithProgress>

namespace Hemera {

class NetworkTransparentOperationPrivate;
/**
 * @class NetworkTransparentOperation
 * @ingroup HemeraCore
 * @headerfile Hemera/hemeranetworktransparentoperation.h <HemeraCore/NetworkTransparentOperation>
 *
 * @brief A server interface for a network transparent Hemera::Operation
 *
 * The Network Transparent Operation paradigm is useful to track operations
 * happening in different processes or different machines. It is an instance-based
 * protocol definition which allows to execute and monitor operation over a generic
 * transport.
 *
 * It is composed of two main components: NetworkTransparentOperation and
 * NetworkTransparentOperationClient. The first should be reimplemented by the server,
 * aka the component which is going to execute the Operation, whereas the latter
 * should be reimplemented by the remote caller of the operation to control and monitor it.
 *
 * It is important to note that these classes do not handle any of the transport logic:
 * the server/client implementation must be implemented by the user. However, some
 * convenience subclasses already handle the logic for the transport internally (such as
 * @ref RootOperation). You should reimplement NetworkTransparentOperation only in those
 * cases where you need to implement network transparency over a specific transport.
 * Most of the times you'll be able to get away with convenience classes in Qt5 SDK or
 * Hyperspace.
 *
 * @par Operation IDs and Instances
 * NetworkTransparentOperation adds an ID and an instance ID to its data structure.
 * This is useful when communicating over the transport, as it is indeed possible
 * a client requests more than once the same operation to be executed concurrently.
 * Servers should handle this detail internally to make sure the handshake gets completed
 * correctly.
 *
 * The operation ID should be compliant with @ref Application IDs (RDNS), whereas the
 * instance ID has no format or syntax restriction. This is done because NetworkTransparentOperations
 * might be exposed as Hemera services in the system, hence their IDs must be compliant.
 *
 * @par Parametrization
 * When a new instance of a NetworkTransparentOperation is created, the client might
 * pass over a list of arguments as a QJsonObject. The JSON Object is used to keep a
 * plain text format which should be serializable and interoperable between most
 * transports. It is guaranteed that, when startImpl will be called,
 * @ref parameters will return the parameters passed to the specific instance.
 *
 * @par Return value
 * Optionally, Operations can return to their connected client a QJsonObject.
 * This is achieved by calling @ref setReturnValue before the Operation is marked
 * as finished.
 *
 * @par Communication and protocol
 * For convenience and simplicity, information between operations is passed as JSON objects.
 * This protocol can be easily extended by the server or the implementation, and passed
 * over to @ref processIncomingPayload for default and standard management.
 * It is important to note that processIncomingPayload must be called by the server implementation
 * every time data needs to be passed to the Operation.
 *
 * On the other hand, whenever data needs to be sent, the newOutgoingPayload signal is emitted.
 * This can be either emitted by the base class to transfer status data, or emitted by implementations
 * for transmitting custom data. The server MUST connect to this signal and pass the data over the
 * transport.
 *
 * @note Even though the protocol is inherently based on JSON/QJson, the transport can encode
 *       data as it pleases, as long it is decoded as a QJsonObject on the other end.
 *
 * @note For how the protocol works, no options can be passed to NetworkTransparentOperation - the client
 *       will decide on Options and Policies to adopt.
 *
 * @sa NetworkTransparentOperationClient
 */
class HEMERA_QT5_SDK_EXPORT NetworkTransparentOperation : public Hemera::OperationWithProgress
{
    Q_OBJECT
    Q_DISABLE_COPY(NetworkTransparentOperation)
    Q_DECLARE_PRIVATE(NetworkTransparentOperation)

    Q_PRIVATE_SLOT(d_func(), void sendStatusUpdate())

public:
    /**
     * @brief Default destructor
     */
    virtual ~NetworkTransparentOperation();

    /**
     * @returns The @ref NetworkTransparentOperation id
     */
    QString id() const;
    /**
     * @returns The @ref NetworkTransparentOperation instance id
     */
    QString instanceId() const;
    /**
     * @returns The parameters for this @ref NetworkTransparentOperation instance.
     *
     * @note This method will return a meaningful value only when the operation has been started.
     */
    QJsonObject parameters() const;

protected:
    /**
     * @brief Constructs a new NetworkTransparentOperation.
     *
     * Subclasses of NetworkTransparentOperation will need to pass through the id
     * of the Operation, which is assumed to be static and well-known at creation time
     * (if it's not, there's something totally wrong with your implementation).
     *
     * @p id The Operation ID.
     * @p parent The QObject parent for this Operation.
     */
    explicit NetworkTransparentOperation(const QString &id, QObject *parent = nullptr);
    explicit NetworkTransparentOperation(NetworkTransparentOperationPrivate &dd, const QString &id, QObject *parent = nullptr);

    /**
     * @brief Processes incoming data.
     *
     * Call this function whenever new data is received over the transport.
     *
     * @p payload The received payload.
     */
    void processIncomingPayload(const QJsonObject &payload);
    /**
     * @brief Sets the parameters for this instance.
     *
     * @note Parameters MUST be set by the server before the operation is started.
     *
     * @p parameters The parameters to be used by this instance.
     */
    void setParameters(const QJsonObject &parameters);
    /**
     * @brief Sets the instance ID of this instance.
     *
     * The instance ID must be unique over the connection with the transport to easily
     * identify the correct instance. The server should take care of dispatching data
     * to the correct instance based on its id.
     *
     * @note The instance ID has no format or syntax requirements or restrictions.
     *
     * @note The instance ID MUST be set before the Operation is started, and as early
     *       as possible as a rule of thumb.
     *
     * @p instanceId The instance ID.
     */
    void setInstanceId(const QString &instanceId);
    /**
     * @brief Sets a return value for the operation.
     *
     * The Operation might return a value in form of a QJsonObject. This is completely
     * optional, since in most cases the Operation just needs to report a successful
     * completion.
     *
     * @note The return value MUST be set before the Operation is marked as finished,
     *       or will be discarded.
     *
     * @p value The return value to be passed to the client.
     */
    void setReturnValue(const QJsonObject &value);

Q_SIGNALS:
    /**
     * @brief Processes outgoing data.
     *
     * Emit this signal whenever you need to send custom data over the transport. The
     * server MUST connect to this signal and stream data over the transport accordingly.
     *
     * @p payload The outgoing payload.
     */
    void newOutgoingPayload(const QJsonObject &payload);

};
}

#endif // HEMERA_NETWORKTRANSPARENTOPERATION_H
