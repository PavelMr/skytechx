#ifndef DSSHEADERDIALOG_H
#define DSSHEADERDIALOG_H

#include <QDialog>

namespace Ui {
class DSSHeaderDialog;
}

class DSSHeaderDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DSSHeaderDialog(QWidget *parent, QMap<QString, QString> list);
  ~DSSHeaderDialog();

private slots:
  void on_pushButton_clicked();

private:
  Ui::DSSHeaderDialog *ui;
};

#endif // DSSHEADERDIALOG_H
