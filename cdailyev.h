#ifndef CDAILYEV_H
#define CDAILYEV_H

#include <QtGui>

#include "skcore.h"
#include "cmapview.h"

class CDailyEvItem : public QStandardItem
{
protected:
  bool	operator< ( const QStandardItem & other ) const
  {
    double dec1 = data().toDouble();
    double dec2 = other.data().toDouble();
    if (dec1 > dec2)
      return(true);

    return(false);
  }
};

namespace Ui {
class CDailyEv;
}

class CDailyEv : public QDialog
{
  Q_OBJECT
  
public:
  explicit CDailyEv(QWidget *parent, mapView_t *view);
  ~CDailyEv();
  
protected:
  void changeEvent(QEvent *e);
  void setTitle(void);
  void fillList(void);
  mapView_t m_view;
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_clicked();

private:
  Ui::CDailyEv *ui;
};

#endif // CDAILYEV_H
