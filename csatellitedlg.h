#ifndef CSATELLITEDLG_H
#define CSATELLITEDLG_H

#include "csgp4.h"

#include <QDialog>

namespace Ui {
class CSatelliteDlg;
}

class CSatelliteDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CSatelliteDlg(QWidget *parent = 0);
  ~CSatelliteDlg();

protected:
  void reject() {}

private slots:
  void on_pushButton_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_5_clicked();

  void slotSelChange(QModelIndex &index);

private:
  Ui::CSatelliteDlg *ui;
  void fillList();
  bool save(const QString &name);

  QString m_cSaveQuest;
};

extern QString curSatelliteCatName;

#endif // CSATELLITEDLG_H
