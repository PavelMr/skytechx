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
  void abortAll();

signals:
  void sigDownloadDone(QNetworkReply::NetworkError error, QByteArray &data, pixCacheKey_t &key);
  void sigAbort();

public slots:    

private slots:
  void downloadFinished(QNetworkReply *reply);

private:    
  QNetworkAccessManager m_manager;
};

#endif // URLFILEDOWNLOAD_H
