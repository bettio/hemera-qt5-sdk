#include "hemerafakeparsec.h"

#include <HemeraCore/Operation>
#include <HemeraCore/Literals>

#include <QtCore/QLoggingCategory>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCall>

#include <QtTest/QTest>

Q_LOGGING_CATEGORY(LOG_HEMERA_TEST_FAKEPARSEC, "Hemera::Test::FakeParsec")

namespace Hemera {

namespace Test {

class FakeParsec::Private
{
public:
    Private() {};
};


FakeParsec::FakeParsec(QObject *parent)
    : AsyncInitDBusObject(parent)
    , d(new Private)
{
}

FakeParsec::~FakeParsec()
{
    delete d;
}


void FakeParsec::initImpl()
{
    QVERIFY(QDBusConnection::sessionBus().registerService(Hemera::Literals::literal(Hemera::Literals::DBus::parsecService())));
    QVERIFY(QDBusConnection::sessionBus().registerObject(Hemera::Literals::literal(Hemera::Literals::DBus::parsecPath()), this));

    setReady();
}

void FakeParsec::startApplication(const QString& service)
{
    QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall(service, Literals::literal(Literals::DBus::applicationPath()),
                                                                            Literals::literal(Literals::DBus::applicationInterface()), QStringLiteral("start")));
}

void FakeParsec::stopApplication(const QString& service)
{
    QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall(service, Literals::literal(Literals::DBus::applicationPath()),
                                                                            Literals::literal(Literals::DBus::applicationInterface()), QStringLiteral("stop")));
}

void FakeParsec::quitApplication(const QString& service)
{
    QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall(service, Literals::literal(Literals::DBus::applicationPath()),
                                                                            Literals::literal(Literals::DBus::applicationInterface()), QStringLiteral("quit")));
}

}

}

#ifdef BUILD_AS_STANDALONE_APPS
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Hemera::Test::FakeParsec mm;
    Hemera::Operation *op = mm.init();

    QObject::connect(op, &Hemera::Operation::finished, [op, &mm] {
        if (op->isError()) {
            qCDebug(LOG_HEMERA_TEST_FAKEPARSEC) << "FakeParsec could not be initialized. Error:" << op->errorName() << op->errorMessage();
            QCoreApplication::instance()->exit(1);
        } else {
            qCDebug(LOG_HEMERA_TEST_FAKEPARSEC) << "FakeParsec ready";
        }
    });

    return app.exec();
}
#endif
