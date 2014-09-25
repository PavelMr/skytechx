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
  void readData(bool last);
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
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void slotReadyRead(void);
  void slotDownloadFinished(QNetworkReply*reply);

private:
  Ui::CDownloadMPC *ui;
};

#endif // CDOWNLOADMPC_H
