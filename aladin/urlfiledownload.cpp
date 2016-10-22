#include "urlfiledownload.h"
#include <QDebug>

UrlFileDownload::UrlFileDownload(QObject *parent, QNetworkDiskCache *cache) : QObject(parent)
{    
  connect(&qmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

  qmanager.setCache(cache);
}

void UrlFileDownload::begin(const QString &urlName, const pixCacheKey_t &key)
{
  QUrl url(urlName);  

  QNetworkRequest request(url);

  //request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  QNetworkReply *reply = qmanager.get(request);

  QVariant val;
  val.setValue(key);
  reply->setProperty("user_data0", val);
}

void UrlFileDownload::downloadFinished(QNetworkReply *reply)
{  
  pixCacheKey_t key = reply->property("user_data0").value<pixCacheKey_t>();

  //qDebug() << "from cache : " << reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool();
  //qDebug() << "   " <<reply->error() << reply->errorString() << reply->bytesAvailable() << key.level << key.pix;

  if (reply->error() == QNetworkReply::NoError)
  {
    //qDebug() << reply->bytesAvailable();
    QByteArray data = reply->readAll();   

    emit sigDownloadDone(reply->error(), data, key);
  }
  else
  {
    emit sigDownloadDone(reply->error(), QByteArray(), key);
  }

  reply->deleteLater();  
}
