#include "urlfiledownload.h"
#include <QDebug>

UrlFileDownload::UrlFileDownload(QObject *parent, QNetworkDiskCache *cache) : QObject(parent)
{    
  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

  m_manager.setCache(cache);
}

void UrlFileDownload::begin(const QString &urlName, const pixCacheKey_t &key)
{
  QUrl url(urlName);  

  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

  QNetworkReply *reply = m_manager.get(request);

  connect(this, SIGNAL(sigAbort()), reply, SLOT(abort()));

  QVariant val;
  val.setValue(key);
  reply->setProperty("user_data0", val);
}

void UrlFileDownload::abortAll()
{
  emit sigAbort();
}

void UrlFileDownload::downloadFinished(QNetworkReply *reply)
{    
  pixCacheKey_t key = reply->property("user_data0").value<pixCacheKey_t>();  

  //QVariant fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);

  if (reply->error() == QNetworkReply::NoError)
  {    
    QByteArray data = reply->readAll();   

    emit sigDownloadDone(reply->error(), data, key);
  }
  else
  {    
    emit sigDownloadDone(reply->error(), QByteArray(), key);
  }

  reply->deleteLater();  
}
