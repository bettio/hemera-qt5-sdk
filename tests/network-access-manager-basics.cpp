#include <HemeraTest/FakeHeadlessApplication>
#include <HemeraTest/TestWithApplicationEnvironment>

#include <HemeraCore/AsyncInitDBusObject>
#include <HemeraCore/NetworkAccessManager>
#include <HemeraCore/Operation>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QTemporaryDir>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtTest/QSignalSpy>
#include <QtTest/QTest>


using namespace Hemera;

// We need an application to access resource paths
class TestNetworkAccessManager : public Hemera::Test::TestWithApplicationEnvironment
{
    Q_OBJECT

public:
    TestNetworkAccessManager(QObject *parent = nullptr)
        : TestWithApplicationEnvironment(parent)
        , m_dir(new QTemporaryDir)
        , m_nam(new NetworkAccessManager(this))
    {
    }
    virtual ~TestNetworkAccessManager()
    {
        delete m_dir;
    };

private Q_SLOTS:
    void initTestCase();        // inits the necessaary for test to execute

    void testAccessResources();
    void testQmlDir();

    void cleanupTestCase();

private:
    QTemporaryDir *m_dir;
    NetworkAccessManager *m_nam;
    Hemera::Test::FakeHeadlessApplication *m_fakeApplication;
};


void TestNetworkAccessManager::initTestCase()
{
    TestWithApplicationEnvironment::initTestCaseImpl();

    // Set the current dir to a temp dir. This way, we'll be able to inject and access resources
    QDir::setCurrent(m_dir->path());

    m_fakeApplication = new Hemera::Test::FakeHeadlessApplication;

    QSignalSpy applicationStatus(m_fakeApplication, SIGNAL(applicationStatusChanged(Hemera::Application::ApplicationStatus)));

    m_fakeApplication->init();

    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Initializing);
    QVERIFY(applicationStatus.wait());
    QCOMPARE(applicationStatus.takeFirst().at(0).value<Hemera::Application::ApplicationStatus>(), Hemera::Application::ApplicationStatus::Stopped);

    QVERIFY(QDBusConnection::sessionBus().interface()->registeredServiceNames().value().contains(QStringLiteral("com.ispirata.Hemera.Tests.FakeHeadlessApplication")));

    QDir tmp(m_dir->path());
    tmp.mkpath(QString::fromLatin1("%1%2.config%2%3%2elements").arg(m_dir->path(), QDir::separator(), Application::id()));

    // Create files
    QFile mainQml(QString::fromLatin1("%1%2.config%2%3%2main.qml").arg(m_dir->path(), QDir::separator(), Application::id()));
    mainQml.open(QIODevice::WriteOnly);
    mainQml.write(QByteArray("import elements\n\nCustomStuff {}\n"));
    mainQml.flush();
    mainQml.close();

    QFile elementQml(QString::fromLatin1("%1%2.config%2%3%2elements%2CustomStuff.qml").arg(m_dir->path(), QDir::separator(), Application::id()));
    elementQml.open(QIODevice::WriteOnly);
    elementQml.write(QByteArray("Component { property string test }\n"));
    elementQml.flush();
    elementQml.close();

    QVERIFY(QFile::exists(QString::fromLatin1("%1%2.config%2%3%2elements%2CustomStuff.qml").arg(m_dir->path(), QDir::separator(), Application::id())));
    QVERIFY(QFile::exists(QString::fromLatin1("%1%2.config%2%3%2main.qml").arg(m_dir->path(), QDir::separator(), Application::id())));

}

void TestNetworkAccessManager::testAccessResources()
{
    QNetworkRequest req(QUrl(QStringLiteral("resource:///main.qml")));
    QNetworkReply *reply = m_nam->get(req);
    if (!reply->isFinished()) {
        QVERIFY(reply->waitForReadyRead(1000));
    }
    QVERIFY(reply->size() > 0);

    req = QNetworkRequest(QUrl(QStringLiteral("resource:///elements/CustomStuff.qml")));
    reply = m_nam->get(req);
    if (!reply->isFinished()) {
        QVERIFY(reply->waitForReadyRead(1000));
    }
    QVERIFY(reply->size() > 0);

    // Fail those, at all
    req = QNetworkRequest(QUrl(QStringLiteral("resource+defaults://elements/CustomStuff.qml")));
    reply = m_nam->get(req);
    if (!reply->isFinished()) {
        QVERIFY(reply->waitForReadyRead(1000));
    }
    QVERIFY(!reply->size());

    req = QNetworkRequest(QUrl(QStringLiteral("resource+application://elements/CustomStuff.qml")));
    reply = m_nam->get(req);
    if (!reply->isFinished()) {
        QVERIFY(reply->waitForReadyRead(1000));
    }
    QVERIFY(!reply->size());

    // Operation not supported
    req = QNetworkRequest(QUrl(QStringLiteral("resource:///elements/CustomStuff.qml")));
    reply = m_nam->head(req);
    if (!reply->isFinished()) {
        QTest::qSleep(100);
    }
    QVERIFY(!reply->size());
}

void TestNetworkAccessManager::testQmlDir()
{
    QNetworkRequest req = QNetworkRequest(QUrl(QStringLiteral("resource:///elements/qmldir")));
    QNetworkReply *reply = m_nam->get(req);
    if (!reply->isFinished()) {
        QVERIFY(reply->waitForReadyRead(1000));
    }
    QVERIFY(reply->size() > 0);
    QCOMPARE(QString::fromLatin1(reply->readAll()).remove(QLatin1Char('\n')), QStringLiteral("CustomStuff 1.0 CustomStuff.qml"));
}

void TestNetworkAccessManager::cleanupTestCase()
{
    Test::cleanupTestCaseImpl();
}

QTEST_MAIN(TestNetworkAccessManager)
#include "network-access-manager-basics.cpp.moc.hpp"
