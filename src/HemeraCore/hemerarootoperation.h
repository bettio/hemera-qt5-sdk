#ifndef HEMERA_ROOTOPERATION_H
#define HEMERA_ROOTOPERATION_H

#include <HemeraCore/NetworkTransparentOperation>

#include <QtCore/QCoreApplication>

/**
 * @addtogroup GlobalMacros
 *
 * @{
 */

/**
 * @brief Declares a Root Operation Worker.
 *
 * Place this macro at the end of the class implementation file of your RootOperation
 * to have the worker automatically generated. A QCoreApplication will be
 * spawned and the Operation will be executed.
 *
 * @p ClassName The class name of your RootOperation.
 * @p OperationId The operation ID of your RootOperation, as a string.
 */

#define ROOT_OPERATION_WORKER(ClassName,OperationId) \
int main(int argc, char *argv[])\
{\
    QCoreApplication app(argc, argv);\
\
    ClassName *op = new ClassName(QStringLiteral(OperationId));\
    return Hemera::RootOperation::rootOperationMain(op); \
}

/** @} */

namespace Hemera {

class RootOperationPrivate;

/**
 * @class RootOperation
 * @ingroup HemeraCore
 * @headerfile Hemera/hemerarootoperation.h <HemeraCore/RootOperation>
 *
 * @brief A server interface for an operation executed by root.
 *
 * Hemera Applications are never run as root for a number of reasons, among which the fact that it
 * would pose a security threat. However, at times it might be necessary to perform some operations as root.
 *
 * RootOperation leverages NetworkTransparentOperation to create a secure worker executing
 * just a small portion of code as an Operation, allowing it to be tracked by the main
 * application via @ref RootOperationClient.
 *
 * @par Implementation
 * RootOperation should be reimplemented as standard Operations. The transport details for
 * NetworkTransparentOperation are already handled inside RootOperation, so you don't need to care.
 * Progress can be reported to the client via the standard @ref OperationWithProgress interface.
 * As it stands, there's nothing different from implementing a generic Operation.
 *
 * It is, however, needed to add @ref ROOT_OPERATION_WORKER to the bottom of your implementation
 * file like this:
 *
 * @par
 * @code
 * class MyRootOperation : public Hemera::RootOperation
 * { [...] }
 *
 * ROOT_OPERATION_WORKER(MyRootOperation, "my.root.operation")
 * @endcode
 *
 * @par Compilation
 * RootOperation must be compiled as its own executable and installed as such. It is <b>STRONGLY</b> recommended
 * to use ha buildsystem, which handles this detail automatically through @ref Hemera::Qml::Settings::RootOperation.
 * If, for any reason, you can't use it, please refer to Hemera's CMake manual.
 *
 * @sa NetworkTransparentOperation
 * @sa RootOperationClient
 */
class HEMERA_QT5_SDK_EXPORT RootOperation : public Hemera::NetworkTransparentOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(RootOperation)
    Q_DECLARE_PRIVATE(RootOperation)

    Q_PRIVATE_SLOT(d_func(), void initServer())

public:
    /**
     * Default destructor
     */
    virtual ~RootOperation();

    static int rootOperationMain(RootOperation *op);

protected:
    /**
     * @brief Constructs a new RootOperation.
     *
     * Subclasses of RootOperation will need to pass through the id
     * of the Operation, which is assumed to be static and well-known at creation time
     * (if it's not, there's something totally wrong with your implementation).
     *
     * @note ExecutionOptions on RootOperation cannot be set due to how the framework is implemented.
     *
     * @p id The Operation ID.
     * @p parent The QObject parent for this Operation.
     */
    explicit RootOperation(const QString &id, QObject *parent = nullptr);
};
}

#endif // HEMERA_ROOTOPERATION_H
