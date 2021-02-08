#ifndef HEMERA_NETWORKDOWNLOADOPERATION_H_
#define HEMERA_NETWORKDOWNLOADOPERATION_H_

#include <QtCore/QByteArray>
#include <QtCore/QTime>
#include <QtCore/QUrl>

#include <QtNetwork/QNetworkRequest>

#include <HemeraCore/OperationWithProgress>

class QFile;
class QNetworkAccessManager;

namespace Hemera {

class HEMERA_QT5_SDK_EXPORT  NetworkDownloadOperation : public Hemera::OperationWithProgress
{
    Q_OBJECT

    Q_PROPERTY(int rate READ rate)

    public:
        NetworkDownloadOperation(const QNetworkRequest &request, QFile *file, QNetworkAccessManager *nam = nullptr, const QByteArray &expectedChecksum = QByteArray(), QObject *parent = nullptr);
        NetworkDownloadOperation(const QUrl &downloadUrl, QFile *file, QNetworkAccessManager *nam = nullptr, QObject *parent = nullptr);
        virtual ~NetworkDownloadOperation();

        int rate() const;

    public Q_SLOTS:
        virtual void startImpl() override final;

    protected:
        void setProgress(int progress, int rate);

    private:
        int m_rate;
        QFile *m_file;
        QNetworkRequest m_req;
        QNetworkAccessManager *m_nam;
        QByteArray m_checksum;
        QUrl m_url;

        QTime m_downloadTime;
};

} // Hemera

#endif
