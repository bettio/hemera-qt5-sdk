/*
 *
 */

#ifndef HEMERA_TESTS_FAKEPARSEC_H
#define HEMERA_TESTS_FAKEPARSEC_H

#include <HemeraCore/AsyncInitDBusObject>

namespace Hemera {
namespace Test {

class FakeParsec : public Hemera::AsyncInitDBusObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FakeParsec)

public:
    explicit FakeParsec(QObject *parent = 0);
    virtual ~FakeParsec();

public Q_SLOTS:
    // Backdoors
    void startApplication(const QString &service);
    void stopApplication(const QString &service);
    void quitApplication(const QString &service);

protected Q_SLOTS:
    void initImpl() Q_DECL_OVERRIDE;

private:
    class Private;
    Private * const d;
};

}
}

#endif // HEMERA_TESTS_FAKEPARSEC_H
