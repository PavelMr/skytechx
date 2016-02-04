#ifndef CINSERTFRMFIELD_H
#define CINSERTFRMFIELD_H

#include <QDialog>

namespace Ui {
  class CInsertFrmField;
}

typedef struct
{
  QString name;
  double  x, y;
} deviceItem_t;

class CInsertFrmField : public QDialog
{
  Q_OBJECT

public:
  explicit CInsertFrmField(QWidget *parent = 0);
  ~CInsertFrmField();

  double m_x;
  double m_y;
  QString m_name;

protected:
  void updateData(void);

private slots:
  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_doubleSpinBox_3_valueChanged(double arg1);

  void on_doubleSpinBox_valueChanged(double arg1);

  void on_doubleSpinBox_2_valueChanged(double arg1);

  void on_pushButton_4_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

  void on_pushButton_5_clicked();

  void on_barlow_valueChanged(double arg1);

private:
  Ui::CInsertFrmField *ui;
};

#endif // CINSERTFRMFIELD_H
