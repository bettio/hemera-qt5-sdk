#ifndef HEMERA_HEMERAASYNCINITDBUSOBJECT_H
#define HEMERA_HEMERAASYNCINITDBUSOBJECT_H

#include <HemeraCore/AsyncInitObject>

#include <QtDBus/QDBusContext>
#include <QtDBus/QDBusVariant>

namespace Hemera {

class AsyncInitDBusObjectPrivate;
/**
 * @class AsyncInitDBusObject
 * @ingroup HemeraCore
 * @headerfile Hemera/hemeraasyncinitdbusobject.h <HemeraCore/AsyncInitDBusObject>
 *
 * @brief An async init object over DBus
 *
 * This class is merely meant for internal used. It represents an AsyncInitObject
 * exportable over DBus, including its properties. Most of the times, you'll want to use
 * AsyncInitObject instead.
 *
 * @sa AsyncInitObject
 */
class HEMERA_QT5_SDK_EXPORT AsyncInitDBusObject : public Hemera::AsyncInitObject, protected QDBusContext
{
    Q_OBJECT
    Q_DISABLE_COPY(AsyncInitDBusObject)
    Q_DECLARE_PRIVATE_D(d_h_ptr, AsyncInitDBusObject)
    Q_CLASSINFO("D-Bus Interface", "com.ispirata.Hemera.DBusObject")

    Q_PRIVATE_SLOT(d_func(), void propertyChanged());

public:
    virtual ~AsyncInitDBusObject();

public Q_SLOTS:
    QVariantMap allProperties() const;

protected:
    explicit AsyncInitDBusObject(QObject *parent);
    explicit AsyncInitDBusObject(AsyncInitDBusObjectPrivate &dd, QObject *parent);

Q_SIGNALS:
    void propertiesChanged(const QVariantMap &changed);
};

}

#endif // HEMERA_HEMERAASYNCINITDBUSOBJECT_H
