#ifndef URLFILEDOWNLOAD_H
#define URLFILEDOWNLOAD_H

#include "aladin.h"

#include <QtNetwork>

class UrlFileDownload : public QObject
{
  Q_OBJECT
public:
  explicit UrlFileDownload(QObject *parent, QNetworkDiskCache *cache);
  void begin(const QString &urlName, const pixCacheKey_t &key);

signals:
  void sigDownloadDone(QNetworkReply::NetworkError error, QByteArray &data, pixCacheKey_t &key);

public slots:

private slots:
  void downloadFinished(QNetworkReply *reply);

private:  
  //pixCacheKey_t m_key;
  QNetworkAccessManager qmanager;
};

#endif // URLFILEDOWNLOAD_H
