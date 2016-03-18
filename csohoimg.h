#ifndef CSOHOIMG_H
#define CSOHOIMG_H

#include <QDialog>
#include "cimageview.h"
#include "curlfile.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>


namespace Ui {
class CSohoImg;
}

class CSohoImg : public QDialog
{
  Q_OBJECT

public:
  explicit CSohoImg(QWidget *parent = 0);
  ~CSohoImg();

protected:
  void changeEvent(QEvent *e);
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);

  QList <urlItem_t>      tListUrl;
  QNetworkAccessManager *manager;
  uint                   m_hash;

private slots:
  void on_pushButton_2_clicked();
  void slotDownFinished(QNetworkReply *reply);

  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_clicked();

private:
  Ui::CSohoImg *ui;
};

#endif // CSOHOIMG_H
