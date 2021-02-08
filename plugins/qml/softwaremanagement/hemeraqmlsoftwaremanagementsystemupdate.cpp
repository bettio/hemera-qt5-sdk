#include "hemeraqmlsoftwaremanagementsystemupdate.h"

#include <QtCore/QSharedData>
#include <QtCore/QVersionNumber>


namespace Hemera {

namespace Qml {

namespace SoftwareManagement {

class SystemUpdate::Private
{
public:
    Private() { }

    Hemera::SoftwareManagement::SystemUpdate proxySystemUpdate;
};

SystemUpdate::SystemUpdate(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

SystemUpdate::~SystemUpdate()
{
    delete d;
}

void SystemUpdate::setSystemUpdate(const Hemera::SoftwareManagement::SystemUpdate &systemUpdate)
{
    d->proxySystemUpdate = systemUpdate;
    Q_EMIT systemUpdateChanged();
}

QString SystemUpdate::changelog() const
{
    return d->proxySystemUpdate.changelog();
}

QString SystemUpdate::readme() const
{
    return d->proxySystemUpdate.readme();
}

quint64 SystemUpdate::downloadSize() const
{
    return d->proxySystemUpdate.downloadSize();
}

Hemera::SoftwareManagement::SystemUpdate::UpdateType SystemUpdate::updateType() const
{
    return d->proxySystemUpdate.updateType();
}

bool SystemUpdate::isValid() const
{
    return d->proxySystemUpdate.isValid();
}

QString SystemUpdate::applianceVersion() const
{
    return d->proxySystemUpdate.applianceVersion();
}

}

}

}

#include "moc_hemeraqmlsoftwaremanagementsystemupdate.cpp"
