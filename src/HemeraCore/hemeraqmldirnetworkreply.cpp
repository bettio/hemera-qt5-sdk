/*
 *
 */

#include "hemeraqmldirnetworkreply_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QDir>
#include <QtCore/QSet>

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Hemera
{

QmlDirNetworkReply::QmlDirNetworkReply(QNetworkAccessManager::Operation op, const QNetworkRequest &req, const QStringList &dirs, QObject* parent)
    : QNetworkReply(parent)
    , m_dirs(dirs)
{
    setRequest(req);
    setUrl(req.url());
    setOperation(op);
    setFinished(true);
    QNetworkReply::open(QIODevice::ReadOnly);

    QStringList qmlFiles;

    for (const QString &path : m_dirs) {
        QDir dir(path);
        qmlFiles << dir.entryList(QStringList() << QStringLiteral("*.qml"));
    }
    // Remove dupes
    qmlFiles = qmlFiles.toSet().toList();

    QString qmldirData;
    for (const QString &qmlFile : qmlFiles) {
        qmldirData.append(QStringLiteral("%1 1.0 %1.qml\n").arg(qmlFile.split(QStringLiteral(".qml")).first()));
    }

    m_data = qmldirData.toLatin1();
    m_dataStream = new QDataStream(m_data);

    // Do we have a way to compute this? Leave it out, for now.
    // setHeader(QNetworkRequest::LastModifiedHeader, fi.lastModified());
    setHeader(QNetworkRequest::ContentLengthHeader, m_data.size());

    QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "downloadProgress", Qt::QueuedConnection,
                              Q_ARG(qint64, m_data.size()), Q_ARG(qint64, m_data.size()));
    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}

QmlDirNetworkReply::~QmlDirNetworkReply()
{
    delete m_dataStream;
}

void QmlDirNetworkReply::abort()
{
    // Nothing to do...
}

bool QmlDirNetworkReply::isSequential() const
{
    return true;
}

qint64 QmlDirNetworkReply::size() const
{
    return m_data.size();
}

qint64 QmlDirNetworkReply::readData(char* data, qint64 maxlen)
{
    return m_dataStream->readRawData(data, maxlen);
}


NotFoundNetworkReply::NotFoundNetworkReply(const QNetworkRequest &req, QObject* parent)
    : QNetworkReply(parent)
{
    setRequest(req);
    setUrl(req.url());
    setFinished(true);
    setError(QNetworkReply::ContentNotFoundError, tr("The resource %1 was not found. Please check your installation.")
                                                  .arg(req.url().toDisplayString()));

    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}

NotFoundNetworkReply::~NotFoundNetworkReply()
{
}

void NotFoundNetworkReply::abort()
{
    // Nothing to do...
}

bool NotFoundNetworkReply::isSequential() const
{
    return false;
}

qint64 NotFoundNetworkReply::size() const
{
    return 0;
}

qint64 NotFoundNetworkReply::readData(char* data, qint64 maxlen)
{
    return 0;
}

}
