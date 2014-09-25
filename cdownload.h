#ifndef CDOWNLOAD_H
#define CDOWNLOAD_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include "skcore.h"
#include "curlfile.h"

class CDownload : public QObject
{
  Q_OBJECT
public:
  explicit CDownload(QObject *parent = 0);
  void     begin(QString url, QString fileName);

protected:
  QNetworkAccessManager manager;
  QString               m_fileName;
  
signals:
  void sigFinished(void);
  void sigProgress(int id, int val);
  void sigError(QString str);
  
public slots:
  void slotProgress(qint64 recv ,qint64 total);
  void slotDownloadFinished(QNetworkReply*reply);
  
};


#endif // CDOWNLOAD_H
