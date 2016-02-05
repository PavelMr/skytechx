#ifndef MOONLESSNIGHTSDLG_H
#define MOONLESSNIGHTSDLG_H

#include "castro.h"

#include <QDialog>

namespace Ui {
class MoonlessNightsDlg;
}

class MoonlessNightsDlg : public QDialog
{
  Q_OBJECT

public:
  explicit MoonlessNightsDlg(QWidget *parent, mapView_t *view);
  ~MoonlessNightsDlg();
  double m_jd;

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

private:
  Ui::MoonlessNightsDlg *ui;
  mapView_t m_view;
  double m_startJD;

  void fillList();
};

#endif // MOONLESSNIGHTSDLG_H
