#ifndef CRELESEINFO_H
#define CRELESEINFO_H

#include <QDialog>

namespace Ui {
class CReleseInfo;
}

class CReleseInfo : public QDialog
{
  Q_OBJECT

public:
  explicit CReleseInfo(QWidget *parent = 0);
  ~CReleseInfo();

private slots:
  void on_pushButton_clicked();

private:
  Ui::CReleseInfo *ui;
};

#endif // CRELESEINFO_H
