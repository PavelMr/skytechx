#ifndef CSATEVENTS_H
#define CSATEVENTS_H

#include <QtGui>

#include "skcore.h"
#include "castro.h"
#include "cmapview.h"
#include "csatxyz.h"

namespace Ui {
  class CSatEvents;
}

class CSatEvents : public QDialog
{
  Q_OBJECT

public:
  explicit CSatEvents(QWidget *parent, mapView_t *view);
  ~CSatEvents();

  double    m_time;
  double    m_ra;
  double    m_dec;
  double    m_fov;

protected:
  void solve(double jd, int pln);
  mapView_t m_view;
  mapView_t m_viewOrig;
  double    m_jd;
  int       m_planet;

private slots:
  void on_pushButton_3_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

private:
  Ui::CSatEvents *ui;
};

#endif // CSATEVENTS_H
