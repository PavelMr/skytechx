#ifndef CDOWNLOADFILE_H
#define CDOWNLOADFILE_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class CDownloadFile;
}

class CDownloadFile : public QDialog
{
  Q_OBJECT

public:
  explicit CDownloadFile(QWidget *parent, const QString &urlFile);
  ~CDownloadFile();
  QString m_filePath;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void slotReadyRead();
  void slotDownloadFinished(QNetworkReply *reply);
  void slotProgress(qint64 current, qint64 total);
private:
  Ui::CDownloadFile *ui;
  QNetworkAccessManager m_manager;
  QNetworkReply *m_reply;
  quint64 m_current;
  QByteArray m_data;
};

#endif // CDOWNLOADFILE_H
