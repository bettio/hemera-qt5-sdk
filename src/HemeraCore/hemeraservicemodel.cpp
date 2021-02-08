#include "hemeraservicemodel.h"

#include <HemeraCore/Operation>
#include <HemeraCore/ServiceManager>

#include <QtCore/QUrl>
#include <QtCore/QPointer>

namespace Hemera {

class ServiceModel::Private
{
public:
    Private(ServiceModel *q) : q(q) {}

    ServiceModel *q;

    QPointer< ServiceManager > manager;
    ModelDisplayType type;
    ServiceInfoList services;

    void refreshServiceInfoList();
};

void ServiceModel::Private::refreshServiceInfoList()
{
    q->beginRemoveRows(QModelIndex(), 0, q->rowCount());
    services.clear();
    q->endRemoveRows();
    q->beginResetModel();
    q->endResetModel();
    if (!manager.isNull()) {
        q->beginInsertRows(QModelIndex(), q->rowCount(), q->rowCount());
        switch (type) {
            case ModelDisplayType::Applications:
                services = manager->services(ServiceInfo::ServiceType::Application);
                break;
            case ModelDisplayType::OrbitalApplications:
                services = manager->orbitalApplications();
                break;
            case ModelDisplayType::Orbits:
                services = manager->services(ServiceInfo::ServiceType::Orbit);
                break;
            case ModelDisplayType::AllAvailable:
                services = manager->availableServices();
                break;
        }
        q->endInsertRows();
    }
}


ServiceModel::ServiceModel(ServiceManager* manager, ServiceModel::ModelDisplayType type, QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
    d->manager = manager;
    d->type = type;

    connect(d->manager.data(), SIGNAL(servicesChanged()), this, SLOT(refreshServiceInfoList()));
    connect(d->manager.data(), SIGNAL(destroyed()), this, SLOT(refreshServiceInfoList()));
    d->refreshServiceInfoList();
}

ServiceModel::ServiceModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
    d->manager = new ServiceManager(parent);
    d->type = ModelDisplayType::AllAvailable;

    connect(d->manager.data(), SIGNAL(servicesChanged()), this, SLOT(refreshServiceInfoList()));
    connect(this, SIGNAL(destroyed()), d->manager.data(), SLOT(deleteLater()));

    connect(d->manager.data()->init(), &Hemera::Operation::finished, [this] (Hemera::Operation *op) {
        if (op->isError()) {
            return;
        }

        // Populate our model
        d->refreshServiceInfoList();
    });
}

ServiceModel::~ServiceModel()
{
    delete d;
}

ServiceModel::ModelDisplayType ServiceModel::modelDisplayType() const
{
    return d->type;
}

void ServiceModel::setModelDisplayType(ServiceModel::ModelDisplayType type)
{
    d->type = type;
    if (d->manager->isReady()) {
        d->refreshServiceInfoList();
    }
}

int ServiceModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || d->manager.isNull()) {
        return 0;
    } else {
        return d->services.count();
    }
}

bool ServiceModel::canDropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&) const
{
    // In this overload, we can't.
    return false;
}

Qt::ItemFlags ServiceModel::flags(const QModelIndex& index) const
{
    if (index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::NoItemFlags;
}

QVariant ServiceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= d->services.count() || index.row() < 0 || d->manager.isNull()) {
        return QVariant();
    }

    ServiceInfo service = d->services.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return service.name();
        case AssociatedOrbitRole:
            if (service.type() == ServiceInfo::ServiceType::Orbit) {
                return service.id();
            } else if (service.type() == ServiceInfo::ServiceType::Application) {
                return d->manager->orbitForApplication(service).id();
            } else {
                return QString();
            }
            break;
        case DescriptionRole:
            return service.description();
        case HandledMimeTypesRole:
            return service.handledMimeTypes();
        case IconRole:
            return service.icon();
        case IdRole:
            return service.id();
        case NameRole:
            return service.name();
        default:
            break;
    }

    return QVariant();
}

QHash< int, QByteArray > ServiceModel::roleNames() const
{
    QHash< int, QByteArray > roles = QAbstractItemModel::roleNames();

    roles.insert(AssociatedOrbitRole, "associatedOrbit");
    roles.insert(DescriptionRole, "description");
    roles.insert(HandledMimeTypesRole, "handledMimeTypes");
    roles.insert(IconRole, "icon");
    roles.insert(IdRole, "serviceId");
    roles.insert(NameRole, "serviceName");

    return roles;
}

}

#include "moc_hemeraservicemodel.cpp"
