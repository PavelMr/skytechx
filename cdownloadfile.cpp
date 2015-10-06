#include "cdownloadfile.h"
#include "ui_cdownloadfile.h"
#include "curlfile.h"
#include "skcore.h"

#include <QDebug>

CDownloadFile::CDownloadFile(QWidget *parent, const QString &urlFile) :
  QDialog(parent),
  ui(new Ui::CDownloadFile)
{
  ui->setupUi(this);

  CUrlFile u;
  QList <urlItem_t> url;

  u.readFile(urlFile, &url);

  foreach (urlItem_t item, url)
  {
    ui->comboBox->addItem(item.name, item.url);
  }
}

CDownloadFile::~CDownloadFile()
{
  delete ui;
}

void CDownloadFile::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CDownloadFile::on_pushButton_2_clicked()
{
  if (ui->comboBox->currentIndex() == -1)
  {
    return;
  }

  QNetworkRequest request(ui->comboBox->currentData().toString());
  QNetworkReply *reply = m_manager.get(request);

  m_reply = reply;
  m_current = 0;

  ui->pushButton_2->setEnabled(false);
  ui->comboBox->setEnabled(false);

  connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
  connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slotProgress(qint64,qint64)));
  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

void CDownloadFile::slotReadyRead()
{
  m_data += m_reply->readAll();
}

void CDownloadFile::slotDownloadFinished(QNetworkReply *reply)
{
  qDebug() << reply << reply->error();
  if (reply->error() == QNetworkReply::NoError)
  {
    QFileDialog dlg(this);

    m_filePath = dlg.getSaveFileName(this, tr("Save TLE File"), QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/tle/unnamed.tle", "TLE (*.tle)");
    if (!m_filePath.isEmpty())
    {
      QFile f(m_filePath);

      if (f.open(SkFile::WriteOnly))
      {
        f.write(m_data);
        f.close();
      }
    }

    done(DL_OK);
  }
  else
  { // error
    if (reply->error() != QNetworkReply::OperationCanceledError)
    {
      msgBoxError(this, reply->errorString());
    }
  }
  reply->deleteLater();
  done(DL_OK);
}

void CDownloadFile::slotProgress(qint64 current, qint64 total)
{
  ui->progressBar->setRange(0, total / 1000);
  ui->progressBar->setValue(current / 1000);
}
