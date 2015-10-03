#ifndef CEARTHTOOLS_H
#define CEARTHTOOLS_H

#include <QObject>
#include <QtNetwork>
#include <QtXml>

#include "cparse.h"

#define  ETT_NONE        0
#define  ETT_HEIGHT      1
#define  ETT_TIMEZONE    2
#define  ETT_DST         3

class CEarthTools : public QObject
{
  Q_OBJECT
public:
  explicit CEarthTools(QObject *parent = 0);

  void setCacheFolder(QString path);
  void getHeight(double lon, double lat);
  void getTimeZone(double lon, double lat);
  void getDST(double lon, double lat);

protected:
  QNetworkAccessManager m_manager;
  QString               m_cacheFolder;
  int                   m_downloadType;    // ETT_xxx

  double parseHeight(const QByteArray data, bool &ok);
  double parseTimeZone(const QByteArray data, bool &ok);
  double parseDST(const QByteArray data, bool &ok);

signals:
  void sigDone(bool ok, double value, int ett);

public slots:
  void slotDownloadFinished(QNetworkReply*reply);
};

#endif // CEARTHTOOLS_H
