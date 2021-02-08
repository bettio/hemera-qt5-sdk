#include "hemeranetworkdownloadoperation.h"

#include <HemeraCore/Literals>
#include <HemeraCore/Operation>
#include <HemeraCore/CommonOperations>

#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#define READ_CHUNK 65536

namespace Hemera
{

NetworkDownloadOperation::NetworkDownloadOperation(const QNetworkRequest &request, QFile *file, QNetworkAccessManager *nam, const QByteArray &expectedChecksum, QObject *parent)
    : Hemera::OperationWithProgress(parent)
    , m_rate(0)
    , m_file(file)
    , m_req(request)
    , m_nam(nam)
    , m_checksum(expectedChecksum)
{
    if (!m_nam) {
        m_nam = new QNetworkAccessManager(this);
    }
}

NetworkDownloadOperation::~NetworkDownloadOperation()
{
}

NetworkDownloadOperation::NetworkDownloadOperation(const QUrl &downloadUrl, QFile *file, QNetworkAccessManager *nam, QObject *parent)
    : NetworkDownloadOperation(QNetworkRequest(downloadUrl), file, nam, QByteArray(), parent)
{
    m_url = downloadUrl;
}

void NetworkDownloadOperation::startImpl()
{
    QCryptographicHash *checksumHash = new QCryptographicHash(QCryptographicHash::Sha1);

    // Start downloading
    QNetworkReply *r = m_nam->get(m_req);
    m_downloadTime.start();
    connect(r, &QIODevice::readyRead, this, [this, checksumHash, r] {
        while (r->bytesAvailable() > 0 && m_file->isOpen()) {
            QByteArray buffer = r->read(READ_CHUNK);
            int writtenBytes = m_file->write(buffer);
            if (Q_UNLIKELY(writtenBytes != buffer.size())) {
                qWarning() << "failed to write downloaded data to disk. got: " << buffer.size() << " written: " << writtenBytes;
                setFinishedWithError(QStringLiteral("com.ispirata.Hemera.DownloadError"),
                                     QStringLiteral("failed to write downloaded data to disk."));
                r->abort();
                m_file->close();
                return;
	    }
            checksumHash->addData(buffer);
        }
    });

    connect(r, &QNetworkReply::finished, this, [this, r, checksumHash] {
        m_file->close();

        if (r->error() != QNetworkReply::NoError) {
            m_file->remove();
            if (r->error() == QNetworkReply::OperationCanceledError) {
                // If aborted, we already notified the error."
                qDebug() << "Reply finished due to an abort request. The operation is already finished.";
            } else {
                setFinishedWithError(QStringLiteral("com.ispirata.Hemera.DownloadError"), QString::number(r->error()));
            }
            return;
        }

        // Checksum?
        QByteArray checksum = checksumHash->result().toHex().toLower();
        if (!m_checksum.isEmpty() && (checksum != m_checksum.toLower())) {
            // Ouch.
            setFinishedWithError(QStringLiteral("com.ispirata.Hemera.WrongChecksum"),
                                 QStringLiteral("Downloaded file has wrong checksum!"));
            m_file->remove();
            delete checksumHash;
            return;
        }
        delete checksumHash;

        setFinished();
    });

    connect(r, &QNetworkReply::downloadProgress, this, [this] (qint64 bytesReceived, qint64 bytesTotal) {
        int percent = (bytesTotal != 0) ? ((bytesReceived * 100) / bytesTotal) : 0;
        int rate = (m_downloadTime.elapsed() != 0) ? ((bytesReceived * 1000.0) / m_downloadTime.elapsed()) : 1;

        setProgress(percent, rate);
    });
}

void NetworkDownloadOperation::setProgress(int progress, int rate)
{
    Hemera::OperationWithProgress::setProgress(progress);
    m_rate = rate;
}

int NetworkDownloadOperation::rate() const
{
    return m_rate;
}

} // Hemera
