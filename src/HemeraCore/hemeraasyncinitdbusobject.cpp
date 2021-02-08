#include "hemeraasyncinitdbusobject_p.h"

#include "hemeraasyncinitobject_p.h"

#include <HemeraCore/Literals>

#include <QtCore/QMetaObject>

#include "dbusobjectadaptor.h"

namespace Hemera {

/*
 * Here be Dragons.
 *
 * The main reason why this code exists is because QtDBus sucks, and does not handle DBus properties correctly.
 * Given all of Hemera SDK has to be a very nice and async beast, we want to sync the properties over the bus when needed.
 * This means: at startup (allProperties) and whenever they change. This extension adds another DBus interface which
 * does exactly that. Automagically, any properties exported with READ and NOTIFY from AsyncInitDBusObject's superclass
 * will be tunnelled through the bus.
 * This way, we minimize expensive DBus roundtrips, keep a perfect network transparent object, and have everything
 * handled automagically. Lots of QMetaObject/QMetaProperty/QMetaMethod goodness to achieve that.
 */
const QMetaObject *AsyncInitDBusObjectPrivate::baseMetaObject() const
{
    Q_Q(const AsyncInitDBusObject);

    // Crawl back to AsyncInitObject, to get the correct property offset.
    const QMetaObject *baseMO = q->metaObject();
    while (QLatin1String(baseMO->className()) != QStringLiteral("Hemera::AsyncInitObject")) {
        baseMO = baseMO->superClass();
        if (Q_UNLIKELY(baseMO == nullptr)) {
            return nullptr;
        }
    }

    return baseMO;
}

void AsyncInitDBusObjectPrivate::initHook()
{
    Q_Q(AsyncInitDBusObject);

    const QMetaObject *metaObject = q->metaObject();
    const QMetaObject *baseMO = baseMetaObject();
    QList<QByteArray> signalList;

    if (Q_UNLIKELY(baseMO == nullptr)) {
        q->setInitError(Literals::literal(Literals::Errors::interfaceNotAvailable()),
                        AsyncInitDBusObject::tr("Could not find the base AsyncInitObject. This error should never happen - maybe corrupt memory?"));
    }

    for(int i = baseMO->propertyOffset(); i < metaObject->propertyCount(); ++i) {
        if (metaObject->property(i).hasNotifySignal()) {
            QMetaMethod notifySignal = metaObject->property(i).notifySignal();
            if (signalList.contains(notifySignal.methodSignature())) {
                // Don't connect to the same signal more than once.
                continue;
            }

            QObject::connect(q, notifySignal, q, metaObject->method(metaObject->indexOfSlot("propertyChanged()")));
            signalList.append(notifySignal.methodSignature());
        }
    }

    new DBusObjectAdaptor(q);
    QDBusConnection::systemBus().registerObject(Literals::literal(Literals::DBus::dbusObjectPath()), q);

    Hemera::AsyncInitObjectPrivate::initHook();
}

void AsyncInitDBusObjectPrivate::propertyChanged()
{
    Q_Q(AsyncInitDBusObject);

    QVariantMap result;
    const QMetaObject *metaObject = q->metaObject();
    const QMetaObject *baseMO = baseMetaObject();

    for(int i = baseMO->propertyOffset(); i < metaObject->propertyCount(); ++i) {
        if (q->senderSignalIndex() == metaObject->property(i).notifySignalIndex()) {
            result.insert(QLatin1String(metaObject->property(i).name()), q->property(metaObject->property(i).name()));
        }
    }

    if (!result.isEmpty()) {
        Q_EMIT q->propertiesChanged(result);
    }
}

AsyncInitDBusObject::AsyncInitDBusObject(QObject* parent)
    : AsyncInitObject(*new AsyncInitDBusObjectPrivate(this), parent)
{
}

AsyncInitDBusObject::AsyncInitDBusObject(AsyncInitDBusObjectPrivate& dd, QObject* parent)
    : AsyncInitObject(dd, parent)
{

}

AsyncInitDBusObject::~AsyncInitDBusObject()
{
}

QVariantMap AsyncInitDBusObject::allProperties() const
{
    Q_D(const AsyncInitDBusObject);

    QVariantMap properties;
    const QMetaObject *baseMO = d->baseMetaObject();

    if (Q_UNLIKELY(baseMO == nullptr)) {
        return properties;
    }

    for(int i = baseMO->propertyOffset(); i < metaObject()->propertyCount(); ++i) {
        properties.insert(QLatin1String(metaObject()->property(i).name()), property(metaObject()->property(i).name()));
    }

    return properties;
}

}

#include "moc_hemeraasyncinitdbusobject.cpp"
