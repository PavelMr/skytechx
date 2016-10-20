#include "urlfiledownload.h"
#include <QDebug>

UrlFileDownload::UrlFileDownload(QObject *parent) : QObject(parent)
{
}

void UrlFileDownload::begin(const QString &urlName, const pixCacheKey_t &key)
{
  QUrl url(urlName);

  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

  QNetworkRequest request(url);
  m_manager.get(request);
  m_key = key;
}

void UrlFileDownload::downloadFinished(QNetworkReply *reply)
{
  //qDebug() << reply->error() << reply->errorString();

  if (reply->error() == QNetworkReply::NoError)
  {
    //qDebug() << reply->bytesAvailable();
    QByteArray data = reply->readAll();

    emit sigDownloadDone(reply->error(), data, m_key);
  }
  else
  {
    emit sigDownloadDone(reply->error(), QByteArray(), m_key);
  }

  reply->deleteLater();
  deleteLater();
}
