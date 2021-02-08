/*
 *
 */

#ifndef HEMERA_QMLDIRNETWORKREPLY_H
#define HEMERA_QMLDIRNETWORKREPLY_H

#include "hemeranetworkaccessmanager.h"

#include <HemeraCore/Global>

#include <QtCore/QStringList>

#include <QtNetwork/QNetworkReply>

class QDataStream;

namespace Hemera
{

class HEMERA_QT5_SDK_NO_EXPORT QmlDirNetworkReply : public QNetworkReply
{
    Q_OBJECT
    Q_DISABLE_COPY(QmlDirNetworkReply)

public:
    explicit QmlDirNetworkReply(QNetworkAccessManager::Operation op, const QNetworkRequest &req, const QStringList &dirs, QObject* parent = 0);
    virtual ~QmlDirNetworkReply();

    virtual void abort();
    virtual qint64 readData(char* data, qint64 maxlen) override final;

    virtual bool isSequential() const override final;
    qint64 size() const override final;

private:
    QByteArray m_data;
    QDataStream *m_dataStream;
    QStringList m_dirs;
};

class HEMERA_QT5_SDK_NO_EXPORT NotFoundNetworkReply : public QNetworkReply
{
    Q_OBJECT
    Q_DISABLE_COPY(NotFoundNetworkReply)

public:
    explicit NotFoundNetworkReply(const QNetworkRequest &req, QObject* parent = 0);
    virtual ~NotFoundNetworkReply();

    virtual void abort();
    virtual qint64 readData(char* data, qint64 maxlen) override final;

    virtual bool isSequential() const override final;
    qint64 size() const override final;
};

}

#endif // HEMERA_QMLDIRNETWORKREPLY_H
