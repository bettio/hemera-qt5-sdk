#ifndef HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT_H
#define HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT_H

#include <HemeraCore/OperationWithProgress>

#include <QtCore/QJsonObject>

namespace Hemera {

class NetworkTransparentOperationClientPrivate;
/**
 * @class NetworkTransparentOperationClient
 * @ingroup HemeraCore
 * @headerfile Hemera/hemeranetworktransparentoperationclient.h <HemeraCore/NetworkTransparentOperationClient>
 *
 * @brief A client interface for a network transparent Hemera::Operation
 *
 * @note Please refer to @ref NetworkTransparentOperation to learn about the Network
 *       Transparent Operation paradigm.
 *
 * NetworkTransparentOperationClient instantiates, controls and monitors a NetworkTransparentOperation
 * remotely. The server/client transport implementation must be implemented by the user. However, some
 * convenience subclasses already handle the logic for the transport internally (such as
 * @ref RootOperation). You should reimplement NetworkTransparentOperationClient only in those
 * cases where you need to implement network transparency over a specific transport.
 * Most of the times you'll be able to get away with convenience classes in Qt5 SDK or
 * Hyperspace.
 *
 * @par Usage
 * Differently from Operation, you can instantiate a NetworkTransparentOperationClient directly,
 * provided you associate a valid transport to it, or you can use a subclass which implements a
 * transport logic already (such as @ref RootOperationClient). The start logic is already implemented
 * and provides all that's needed for achieving Network Transparency with the server - only the
 * transport needs to be managed.
 *
 * @par Operation IDs and Instances
 * When creating a new NetworkTransparentOperationClient, you need to specify the ID
 * of the remote operation, so that the transport can identify its remote access
 * endpoint. The instance ID detail is handled remotely, and you will be able
 * to know the instance ID this Client refers to once the Operation is connected.
 *
 * @par Connection to Operation
 * Upon start, an additional step will be performed under the hood. For Operation users,
 * this changes no semantics compared to standard Operation usage (including ExplicitStartOption).
 *
 * Developers reimplementing NetworkTransparentOperationClient for a new transport, instead,
 * MUST call setTransportReady whenever the transport is ready to be used. The Operation will be
 * started only when the transport is ready. When transport is marked as ready, it is assumed something
 * is connected to @ref newOutgoingPayload and will process it.
 *
 * Regardless, as soon as the NetworkTransparentOperationClient object is instantiated, the developer
 * should strive to make the transport ready as soon as possible.
 *
 * The @ref ExplicitStartOption is managed separately from the transport, and the developer should
 * not care about it.
 *
 * @sa NetworkTransparentOperation
 */
class HEMERA_QT5_SDK_EXPORT NetworkTransparentOperationClient : public OperationWithProgress
{
    Q_OBJECT
    Q_DISABLE_COPY(NetworkTransparentOperationClient)
    Q_DECLARE_PRIVATE(NetworkTransparentOperationClient)

public:
    /**
     * @brief Constructs a new NetworkTransparentOperationClient.
     *
     * The ID must be specified to allow the transport to connect to the correct endpoint.
     *
     * @p id The Operation ID.
     * @p parameters Optional parameters to be passed to the Operation.
     * @p parent The QObject parent for this Operation.
     */
    explicit NetworkTransparentOperationClient(const QString &id, const QJsonObject &parameters = QJsonObject(), QObject *parent = nullptr);
    /**
     * @brief Constructs a new NetworkTransparentOperationClient.
     *
     * Specialized constructor, allows to set ExecutionOptions on the Operation.
     *
     * @sa NetworkTransparentOperationClient
     */
    explicit NetworkTransparentOperationClient(const QString &id, ExecutionOptions options = NoOptions,
                                               const QJsonObject &parameters = QJsonObject(), QObject *parent = nullptr);
    /**
     * @brief Default destructor
     */
    virtual ~NetworkTransparentOperationClient();

    /**
     * @returns Whether the client is currently connected or not.
     */
    bool isConnected() const;

    /**
     * @returns The Operation ID of the remote Operation.
     */
    QString remoteOperationId() const;
    /**
     * @returns The Instance ID of the Operation.
     *
     * @note This method will return a meaningful value only when the Client is connected.
     */
    QString instanceId() const;
    /**
     * @returns The return value from the Operation.
     *
     * This field is optional and might or might not be set by the Operation.
     *
     * @note This method will return a meaningful value only when the Operation has
     *       finished successfully.
     */
    QJsonObject returnValue() const;

protected:
    explicit NetworkTransparentOperationClient(NetworkTransparentOperationClientPrivate &dd, const QString &id, ExecutionOptions options,
                                               const QJsonObject &parameters = QJsonObject(), QObject *parent = nullptr);

    /**
     * @brief Processes incoming data.
     *
     * Call this function whenever new data is received over the transport.
     *
     * @p payload The received payload.
     */
    void processIncomingPayload(const QJsonObject &payload);
    /**
     * @brief Triggers an operation failure due to a transport error.
     *
     * Equivalent to setFinishedWithError, but specifies the reason for failure was
     * due to an error in communication with the remote operation. This means the operation
     * on the other end might have succeeded.
     * If called before the operation was started, starting the operation will cause
     * an immediate failure
     *
     * @p name The error name.
     * @p message An error message. It might be empty.
     */
    void setTransportError(const QString &name, const QString &message = QString());

protected Q_SLOTS:
    virtual void startImpl() override final;

    /**
     * @brief Notifies the underlying transport is ready for connection
     *
     * Reimplementations of NetworkTransparentOperationClient MUST call this method
     * when the transport is ready to be used and a handshake might be streamed over
     * it. Since when this method is called, it is assumed newOutgoingPayload will
     * always result in something to be sent over the transport.
     */
    void setTransportReady();

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
    /**
     * @brief Emitted upon successful connection to remote Operation.
     */
    void connected();
    /**
     * @brief Emitted upon transport being ready.
     *
     * @note <b>NEVER</b> emit this signal directly! Use @ref setTransportReady instead.
     */
    void transportReady();

private:
    friend class ConnectToNetworkTransparentOperation;
};
}

#endif // HEMERA_NETWORKTRANSPARENTOPERATIONCLIENT_H
