#ifndef CDONATION_H
#define CDONATION_H

#include <QDialog>

namespace Ui {
class CDonation;
}

class CDonation : public QDialog
{
  Q_OBJECT

public:
  explicit CDonation(QWidget *parent = 0);
  ~CDonation();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CDonation *ui;
};

#endif // CDONATION_H
