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

#ifndef CDOWNLOADMPC_H
#define CDOWNLOADMPC_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include "curlfile.h"
#include "casterdlg.h"
#include "ccomdlg.h"

namespace Ui {
class CDownloadMPC;
}

class CDownloadMPC : public QDialog
{
  Q_OBJECT

public:
  explicit CDownloadMPC(QWidget *parent, QList <asteroid_t> *tList);
  explicit CDownloadMPC(QWidget *parent, QList <comet_t> *tList);
  ~CDownloadMPC();

protected:
  void changeEvent(QEvent *e);
  void readData();
  void readMPCLine(QString str);
  void readMPCLineComet(QString str);

  bool                  bIsComet;
  QList <urlItem_t>     tUrl;
  QList <asteroid_t>   *m_tList;
  QList <comet_t>      *m_tListCom;
  QNetworkAccessManager m_manager;
  QNetworkReply        *m_reply;
  QByteArray            m_data;
  int                   m_count;
  bool                  m_bFirstData;
  QString               m_filter;
  bool                  m_firstMatch;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void slotReadyRead(void);
  void slotDownloadFinished(QNetworkReply*reply);

private:
  Ui::CDownloadMPC *ui;
};

#endif // CDOWNLOADMPC_H
