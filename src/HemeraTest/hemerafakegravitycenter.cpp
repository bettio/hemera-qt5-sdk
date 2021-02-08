#include "hemerafakegravitycenter.h"
//#include "parsecadaptor.h"

#include <HemeraCore/Operation>
#include <HemeraCore/Literals>

#include <QtCore/QLoggingCategory>
#include <QtDBus/QDBusConnection>
#include <QtTest/QTest>

Q_LOGGING_CATEGORY(LOG_HEMERA_TEST_FAKEGRAVITYCENTER, "Hemera::Test::FakeGravityCenter")

namespace Hemera {

namespace Test {

class FakeGravityCenter::Private
{
public:
    Private() : lastCookie(0) {};

    QString activeOrbit;
    uint lastCookie;
    QHash< quint16, QPair< QString, QString > > cookieToAppName;
};


FakeGravityCenter::FakeGravityCenter(QObject *parent)
    : AsyncInitDBusObject(parent)
    , d(new Private)
{
}

FakeGravityCenter::~FakeGravityCenter()
{
    delete d;
}


void FakeGravityCenter::initImpl()
{
    QVERIFY(QDBusConnection::systemBus().registerService(Hemera::Literals::literal(Hemera::Literals::DBus::gravityCenterService())));
    QVERIFY(QDBusConnection::systemBus().registerObject(Hemera::Literals::literal(Hemera::Literals::DBus::gravityGalaxyPath()), this));

    //new ParsecAdaptor(this);

    d->activeOrbit = QStringLiteral("none");

    setReady();
}

QString FakeGravityCenter::activeOrbit() const
{
    return d->activeOrbit;
}


QVariantMap FakeGravityCenter::inhibitionReasons()
{
    // Let's build the return type correctly to please QtDBus.
    QVariantMap result;
    for (QHash< quint16, QPair< QString, QString > >::const_iterator i = d->cookieToAppName.constBegin(); i != d->cookieToAppName.constEnd(); ++i) {
        result.insertMulti(i.value().first, i.value().second);
    }

    return result;
}


uint FakeGravityCenter::inhibitOrbitSwitch(const QString &in0, const QString &in1)
{
    if (in1 == QStringLiteral("fail")) {
        sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::declarativeError()), QStringLiteral("ouch!!"));
        return 0;
    }

    for (const QPair< QString, QString > r : d->cookieToAppName) {
        if (r.first == in0) {
            sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::notAllowed()), QStringLiteral("already there"));
            return 0;
        }
    }

    ++d->lastCookie;
    d->cookieToAppName.insert(d->lastCookie, qMakePair(in0, in1));
    Q_EMIT inhibitionReasonsChanged();
    return d->lastCookie;
}

void FakeGravityCenter::releaseOrbitSwitchInhibition(uint in0)
{
    if (!d->cookieToAppName.contains(in0)) {
        sendErrorReply(Hemera::Literals::literal(Hemera::Literals::Errors::badRequest()), QStringLiteral("not there"));
        return;
    }

    d->cookieToAppName.remove(in0);
    Q_EMIT inhibitionReasonsChanged();
}

bool FakeGravityCenter::isOrbitSwitchInhibited() const
{
    return !d->cookieToAppName.isEmpty();
}

QDBusVariant FakeGravityCenter::orbitProperty(const QString &in0, const QString &in1) const
{
    return QDBusVariant();
}


void FakeGravityCenter::requestOrbitSwitch(const QString &orbit)
{
    d->activeOrbit = orbit;
    Q_EMIT activeOrbitChanged();
}

}

}

#ifdef BUILD_AS_STANDALONE_APPS
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Hemera::Test::FakeGravityCenter mc;
    Hemera::Operation *op = mc.init();

    QObject::connect(op, &Hemera::Operation::finished, [op, &mc] {
        if (op->isError()) {
            qCDebug(LOG_HEMERA_TEST_FAKEGRAVITYCENTER) << "FakeGravityCenter could not be initialized. Error:" << op->errorName() << op->errorMessage();
            QCoreApplication::instance()->exit(1);
        } else {
            qCDebug(LOG_HEMERA_TEST_FAKEGRAVITYCENTER) << QStringLiteral("FakeGravityCenter ready");
        }
    });

    return app.exec();
}
#endif
