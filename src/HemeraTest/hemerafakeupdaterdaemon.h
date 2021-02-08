#ifndef HEMERA_TESTS_FAKEUPDATERDAEMON_H
#define HEMERA_TESTS_FAKEUPDATERDAEMON_H

#include <HemeraCore/AsyncInitDBusObject>

#include <QtCore/QStringList>

namespace Hemera {

namespace Test {

class FakeSoftwareManagerDaemon : public Hemera::AsyncInitDBusObject
{
    Q_OBJECT

public:
    explicit FakeSoftwareManagerDaemon(QObject *parent = 0);
    virtual ~FakeSoftwareManagerDaemon();

    QStringList availableUpdates();
    void setUpdatePolicy(uint in0) {};

public Q_SLOTS:
    void triggerAvailableUpdates();

Q_SIGNALS:
    void availableUpdatesChanged(const QStringList &updatesList);

protected Q_SLOTS:
    void initImpl() Q_DECL_OVERRIDE;

private:
    class Private;
    Private * const d;
};

}

}

#endif // HEMERA_TESTS_FAKEUPDATERDAEMON_H
