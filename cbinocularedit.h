#ifndef CBINOCULAREDIT_H
#define CBINOCULAREDIT_H

#include <QDialog>

namespace Ui {
class CBinocularEdit;
}

class CBinocularEdit : public QDialog
{
  Q_OBJECT

public:
  explicit CBinocularEdit(QWidget *parent, bool isNew, const QString &name = "", double diam = 0, double mag = 0, double fov = 0);
  ~CBinocularEdit();

  QString m_name;
  double  m_diam;
  double  m_mag;
  double  m_fov;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CBinocularEdit *ui;
};

#endif // CBINOCULAREDIT_H
