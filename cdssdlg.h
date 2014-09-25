#ifndef CDSSDLG_H
#define CDSSDLG_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include "skcore.h"
#include "curlfile.h"
#include "cmapview.h"
#include "mainwindow.h"

namespace Ui {
class CDSSDlg;
}

class CDSSDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit CDSSDlg(QWidget *parent, double ra, double dec, double jd);
  ~CDSSDlg();
  
protected:
  void changeEvent(QEvent *e);

  double  m_jd;
  double  m_ra;
  double  m_dec;
  double  m_size;
  QString m_file;

  QList <urlItem_t> tUrl;
  
private slots:
  void on_pushButton_3_clicked();
  void slotChangeVal(QString);

  void on_pushButton_2_clicked();

private:
  Ui::CDSSDlg *ui;
};

#endif // CDSSDLG_H
