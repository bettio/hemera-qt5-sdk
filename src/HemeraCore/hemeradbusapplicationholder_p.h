/*
 *
 */

#ifndef HEMERA_DBUSAPPLICATIONHOLDER_H
#define HEMERA_DBUSAPPLICATIONHOLDER_H

#include <HemeraCore/AsyncInitObject>

namespace Hemera {

class DBusApplicationHolder : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusApplicationHolder)
    Q_CLASSINFO("D-Bus Interface", "com.ispirata.Hemera.Application.DBusHolder")

public:
    explicit DBusApplicationHolder(const QString &applicationId, QObject *parent);
    virtual ~DBusApplicationHolder();

public Q_SLOTS:
    void Release();

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;

Q_SIGNALS:
    void released();

private:
    QString m_service;
};
}

#endif // HEMERA_DBUSAPPLICATIONHOLDER_H
