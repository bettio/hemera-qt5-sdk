/*
 *
 */

#ifndef HEMERA_NETWORKACCESSMANAGER_H
#define HEMERA_NETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>

namespace Hemera {

/**
 * @class NetworkAccessManager
 * @ingroup HemeraCore
 * @headerfile Hemera/hemeranetworkaccessmanager.h <HemeraCore/NetworkAccessManager>
 *
 * @brief A QNetworkAccessManager implementing additional Hemera protocols.
 *
 * @b Hemera::NetworkAccessManager is an extension to QNetworkAccessManager which silently
 * handles additional protocols, such as resource:/// and hyperspace:// and more. In your applications,
 * when using a QNetworkAccessManager, you should always prefer @b Hemera::NetworkAccessManager instead.
 *
 * @ref Hemera::NetworkAccessManager does not extend QNetworkAccessManager with any public API,
 * and is completely source compatible back and forth from QNetworkAccessManager.
 *
 * @note All QML files spawned from Hemera's QML engine already make use of this specific
 *       QNetworkAccessManager.
 *
 * @sa Operation
 */
class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
    Q_DISABLE_COPY(NetworkAccessManager)

public:
    explicit NetworkAccessManager(QObject* parent = 0);
    virtual ~NetworkAccessManager();

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice *outgoingData = 0) Q_DECL_OVERRIDE;

protected Q_SLOTS:
    QStringList supportedSchemesImplementation() const;

private:
    class Private;
    Private * const d;
};

}

#endif // HEMERA_NETWORKACCESSMANAGER_H
