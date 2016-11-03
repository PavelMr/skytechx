#ifndef HIPSPROPERTIESDIALOG_H
#define HIPSPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class HIPSPropertiesDialog;
}

class HIPSPropertiesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit HIPSPropertiesDialog(QWidget *parent, const QString &file);
  ~HIPSPropertiesDialog();

private:
  Ui::HIPSPropertiesDialog *ui;

private slots:
  void clickedUrl(const QUrl &url);
  void on_pushButton_clicked();
};

#endif // HIPSPROPERTIESDIALOG_H
