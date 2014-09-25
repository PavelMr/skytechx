#ifndef CLOADINGDLG_H
#define CLOADINGDLG_H

#include <QtGui>
#include <QtCore>

#include "setting.h"
#include "constellation.h"
#include "cdso.h"
#include "tycho.h"
#include "cgscreg.h"
#include "cstarrenderer.h"
#include "cmilkyway.h"
#include "csatxyz.h"
#include "casterdlg.h"
#include "ccomdlg.h"

class CWaitLogo;

class CLoading : public QThread
{
  Q_OBJECT

  public:
    void run(void);

  signals:
    void sigEnd(void);
    void sigProgress(int val);
};


namespace Ui {
class CLoadingDlg;
}

class CLoadingDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CLoadingDlg(QWidget *parent = 0);
  ~CLoadingDlg();

protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);
  void reject() {};

  QPixmap  *img;
  CLoading *thread;
  QMovie   *loadMovie;
  QTime     tm;
  CWaitLogo *m_waitLogo;

private:
  Ui::CLoadingDlg *ui;

public slots:
  void slotEnd(void);
  void slotProgress(int val);
};

#endif // CLOADINGDLG_H
