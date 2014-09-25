#ifndef CGETPROFILE_H
#define CGETPROFILE_H

#include <QDialog>

namespace Ui {
class CGetProfile;
}

class CGetProfile : public QDialog
{
  Q_OBJECT

public:
  explicit CGetProfile(QWidget *parent = 0);
  ~CGetProfile();
  QString m_name;

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CGetProfile *ui;
};

#endif // CGETPROFILE_H
