#ifndef CFINDEREDIT_H
#define CFINDEREDIT_H

#include <QDialog>

namespace Ui {
class CFinderEdit;
}

class CFinderEdit : public QDialog
{
  Q_OBJECT

public:
  explicit CFinderEdit(QWidget *parent,  bool isNew, const QString &name = "", double mag = 1, double fov = 1);
  ~CFinderEdit();
  QString m_name;
  double m_fov;
  double m_mag;

private slots:
  void on_pushButton_4_clicked();

  void on_pushButton_3_clicked();

private:
  Ui::CFinderEdit *ui;
};

#endif // CFINDEREDIT_H
