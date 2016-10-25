#ifndef ALADINPROPERTIESDIALOG_H
#define ALADINPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class AladinPropertiesDialog;
}

class AladinPropertiesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AladinPropertiesDialog(QWidget *parent, const QString &file);
  ~AladinPropertiesDialog();

private:
  Ui::AladinPropertiesDialog *ui;

private slots:
  void clickedUrl(const QUrl &url);
  void on_pushButton_clicked();
};

#endif // ALADINPROPERTIESDIALOG_H
