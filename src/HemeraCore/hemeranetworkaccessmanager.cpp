/*
 *
 */

#include "hemeranetworkaccessmanager.h"

#include "hemeraqmldirnetworkreply_p.h"

#include <HemeraCore/Application>

#include <QtCore/QStringList>

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Hemera
{

class NetworkAccessManager::Private
{
public:
    QStringList allSchemes;
};

NetworkAccessManager::NetworkAccessManager(QObject* parent)
    : QNetworkAccessManager(parent)
    , d(new Private)
{
    // Retrieve supported schemes and cache them initially.
    d->allSchemes = supportedSchemes();
    // Add Hemera schemes
    d->allSchemes << QStringLiteral("resource") << QStringLiteral("resource+defaults") << QStringLiteral("resource+orbit");
}

NetworkAccessManager::~NetworkAccessManager()
{
    delete d;
}

QStringList NetworkAccessManager::supportedSchemesImplementation() const
{
    return d->allSchemes;
}

QNetworkReply *NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &r, QIODevice *outgoingData)
{
    // We want to do as less as possible here. Just rewrite the url to a file:// scheme or delegate entirely.
    QString resourcePath = r.url().toString(QUrl::RemoveScheme);
    QUrl realUrl;
    QString scheme = r.url().scheme();

    if (scheme != QStringLiteral("resource") && scheme != QStringLiteral("resource+application") && scheme != QStringLiteral("resource+defaults")) {
        // Delegate immediately
        return QNetworkAccessManager::createRequest(op, r, outgoingData);
    }

    if (r.url().scheme() == QStringLiteral("resource")) {
        // We are accessing the best possible resource for RW
        realUrl = QUrl::fromLocalFile(Application::resourcePath(resourcePath));
    } else if (r.url().scheme() == QStringLiteral("resource+application")) {
        realUrl = QUrl::fromLocalFile(Application::resourcePath(resourcePath, Application::ResourceDomain::Application, false));
    } else if (r.url().scheme() == QStringLiteral("resource+defaults")) {
        realUrl = QUrl::fromLocalFile(Application::resourcePath(resourcePath, Application::ResourceDomain::Defaults, false));
    }

    if (resourcePath.endsWith(QStringLiteral("/qmldir")) && realUrl.isEmpty()) {
        /* Welcome to hackland! Given QML is not smart enough to understand resource:/// and friends
         * can be actually listed, we need to give it a little help. If we're looking for qmldir, it's
         * 99% likely QQmlEngine is requesting relative-path resources for local imports. Let's give it
         * what it really wants then: list the resource directory, and create on the fly a qmldir file
         * to feed QQmlEngine with.
         */
        QStringList qmlFiles;
        QString resourceDir = [resourcePath] () -> QString { QStringList s = resourcePath.split(QLatin1Char('/')); s.removeLast();
                                                             return s.join(QLatin1Char('/')); }();
        // The first character is a random /
        QStringList dirs = QStringList() << Application::resourcePath(resourceDir, Application::ResourceDomain::Defaults, false)
                                         << Application::resourcePath(resourceDir, Application::ResourceDomain::Application, false)
                                         << Application::resourcePath(resourceDir, Application::ResourceDomain::Orbit, false);

        switch (op) {
            case GetOperation:
                return new QmlDirNetworkReply(op, r, dirs, this);
            case PutOperation:
                // This should be handled as a new file.
            default:
                // Let QNAM fail for us.
                break;
        }
    }

    // If we got this far and the URL is empty, then it wasn't there.
    if (Q_UNLIKELY(realUrl.isEmpty())) {
        return new NotFoundNetworkReply(r, this);
    }

    QNetworkRequest request = r;

    switch (op) {
        case GetOperation:
        case PutOperation:
            request.setUrl(realUrl);
            break;
        default:
            // Let QNAM fail for us.
            break;
    }

    // Now, delegate.
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

}
