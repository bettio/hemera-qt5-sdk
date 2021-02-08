#ifndef HEMERA_SERVICEMODEL_H
#define HEMERA_SERVICEMODEL_H

#include <QtCore/QAbstractListModel>

namespace Hemera {

class ServiceManager;

/**
 * @class ServiceModel
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemeraservicemodel.h <HemeraCore/ServiceModel>
 *
 * @brief Convenience Model to access ServiceManager.
 *
 * ServiceModel is a convenience model for accessing services exposed by @ref ServiceManager in the context of
 * a View. It is convenient for QML use cases, or simply where you want to expose Service data to a View.
 * It abstracts @ref ServiceInfo, and lists a specific category of Services.
 *
 * A ServiceModel needs a @ref ServiceManager to operate, which needs to be already initialized when passed.
 * It does not take ownership of the Manager, which can be safely used outside of the context.
 * Should the manager be deleted, every model's index will be permanently invalid.
 * Also, it is possible to create several ServiceModel out of the same @ref ServiceManager
 *
 * @sa Hemera::ServiceManager
 */
class ServiceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(ServiceModel)

    Q_PRIVATE_SLOT(d, void refreshServiceInfoList())

    Q_PROPERTY(ModelDisplayType modelDisplayType READ modelDisplayType WRITE setModelDisplayType)

public:
    /**
     * @brief Special roles exposed by this model.
     *
     * The list of roles exposed by this model. They're basically a mirror of @ref ServiceInfo
     * members.
     *
     * @sa Hemera::ServiceInfo
     */
    enum Roles {
        /// The type of the service, as in @ref Hemera::ServiceInfo::ServiceType
        ServiceTypeRole = Qt::UserRole + 5, // Start with decent padding
        /// The ID of the service.
        IdRole,
        /// The common name of the service.
        NameRole,
        /// The version of the service, if any.
        VersionRole,
        /// Human-readable description of the service, if any.
        DescriptionRole,
        /// URL to the service's icon, if any.
        IconRole,
        /// A list of mimetypes handled by this service, if any.
        HandledMimeTypesRole,
        /**
         * The ID of the associated orbit for this service. If the service is an orbit,
         * it is equivalent to @IdRole . If the service is an application, it is equivalent
         * to the id of the service returned by @ref ServiceManager::orbitForApplication.
         *
         * @sa ServiceManager::orbitForApplication
         */
        AssociatedOrbitRole,
        /**
         * A variant map of additional data carried by this service. Usually for
         * internal use, it might carry interesting data for what concerns execution
         * or other parameters. Usually, you shall not refer to this unless you have
         * very specific needs.
         */
        ServiceDataRole
    };
    Q_ENUM(Roles)

    /**
     * The type of information which should be displayed by this model.
     *
     * It matches the return of several @ref ServiceManager methods when listing services.
     */
    enum class ModelDisplayType {
        /// Lists all applications on the system. Equivalent to ServiceManager::services(Application)
        Applications,
        /**
         * Lists all applications services which have a directly associated orbit - which means they can
         * be launched with no helpers. Equivalent to ServiceManager::orbitalApplications.
         */
        OrbitalApplications,
        /**
         * Lists all orbits on the system. Equivalent to ServiceManager::services(Orbit).
         */
        Orbits,
        /**
         * Lists all available services on the system. Equivalent to ServiceManager::availableServices().
         */
        AllAvailable
    };
    Q_ENUM(ModelDisplayType)

    /**
     * @brief Constructs a new ServiceModel
     *
     * A ServiceModel needs an already initialized and ready @ref ServiceManager, which will be
     * used as the main source for information retrieval.
     *
     * A ServiceModel is valid for a single category of service types, which have to be specified
     * upon construction. You can, however, create more than one ServiceModel out of the same
     * @ref ServiceManager. The Display Type can be changed at runtime at any time, but the model
     * will be reset upon calling it.
     *
     * @note Passing a non ready @ref ServiceManager might lead to undefined behavior and crashes.
     *
     * @param manager An initialized and ready @ref ServiceManager
     * @param type The type of services this model will expose
     * @param parent The QObject parent for the model
     *
     * @sa setModelDisplayType
     */
    explicit ServiceModel(ServiceManager *manager, ModelDisplayType type, QObject *parent = nullptr);
    /**
     * @brief Constructs a new ServiceModel
     *
     * Initializes a private @ref ServiceManager, which will be owned and deleted by this ServiceModel.
     *
     * A ServiceModel is valid for a single category of service types, which has to be specified
     * by invoking @ref setModelDisplayType. By default, a ServiceModel created this way will
     * list @ref AllAvailable.
     *
     * @param parent The QObject parent for the model
     */
    explicit ServiceModel(QObject *parent = nullptr);
    virtual ~ServiceModel();

    /**
     * @brief The Display Type of this ServiceModel
     *
     * @returns The Display Type of this ServiceModel
     */
    ModelDisplayType modelDisplayType() const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    virtual bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QHash< int, QByteArray > roleNames() const override;

public Q_SLOTS:
    /**
     * @brief Sets the Display Type of this ServiceModel
     *
     * When this method is called, the model will be reset and reloaded.
     *
     * @param type The new ModelDisplayType for this model
     */
    void setModelDisplayType(ModelDisplayType type);

private:
    class Private;
    Private * const d;
};
}

#endif // HEMERA_SERVICEMODEL_H
