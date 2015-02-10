#ifndef CVERSIONCHECK_H
#define CVERSIONCHECK_H

#include <QDialog>
#include <QNetworkReply>

namespace Ui {
class CVersionCheck;
}

class CVersionCheck : public QDialog
{
  Q_OBJECT

public:
  explicit CVersionCheck(QWidget *parent, const QString &newVersion, QNetworkReply::NetworkError error, const QString &errorString);
  ~CVersionCheck();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CVersionCheck *ui;
};

#endif // CVERSIONCHECK_H
