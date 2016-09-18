/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

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
