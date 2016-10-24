/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
  void     beginBkImage(QString url, QString fileName);
  void     beginFile(QString url, QString fileName);

protected:
  QNetworkAccessManager manager;
  QString               m_fileName;

signals:
  void sigFinished(void);
  void sigProgress(qint64 id, int val);
  void sigError(QString str);  
  void sigFileDone(bool ok);

public slots:
  void slotProgress(qint64 recv ,qint64 total);
  void slotDownloadFinished(QNetworkReply*reply);  
  void slotDownloadFileFinished(QNetworkReply*reply);

};


#endif // CDOWNLOAD_H
