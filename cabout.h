#ifndef CABOUT_H
#define CABOUT_H

#include <QDialog>

namespace Ui {
class CAbout;
}

class CAbout : public QDialog
{
  Q_OBJECT

public:
  explicit CAbout(QWidget *parent = 0);
  ~CAbout();

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_clicked();

private:
  Ui::CAbout *ui;
};

#endif // CABOUT_H
