#include "csohoimg.h"
#include "ui_csohoimg.h"
#include "suntexture.h"

static int lastSel = 0;

QImage *g_pSunTexture = NULL;

/////////////////////////////////////
CSohoImg::CSohoImg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSohoImg)
/////////////////////////////////////
{
  ui->setupUi(this);

  setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint)
                                & ~Qt::WindowCloseButtonHint) );

  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/sun.url", &tListUrl);

  for (int i = 0; i < tListUrl.count(); i++)
    ui->comboBox->addItem(tListUrl[i].name);

  if (lastSel >= ui->comboBox->count())
  {
    lastSel = 0;
  }

  if (ui->comboBox->count() != 0)
  {
    ui->comboBox->setCurrentIndex(lastSel);
  }
  else
  {
    ui->widget->setSourceNoImage();
    ui->pushButton_2->setEnabled(false);
  }
}


/////////////////////
CSohoImg::~CSohoImg()
/////////////////////
{
  lastSel = ui->comboBox->currentIndex();
  delete ui;
}


void CSohoImg::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void CSohoImg::resizeEvent(QResizeEvent * /*e*/)
{
  ui->widget->slotReset();
}

////////////////////////////////////////
void CSohoImg::on_pushButton_2_clicked()
////////////////////////////////////////
{
  int v = ui->comboBox->currentIndex();

  if (v >= tListUrl.count() || v < 0)
  {
    //pcDebug->writeError("CSohoImg::on_pushButton_2_clicked()");
    return;
  }

  QUrl url(tListUrl[v].url);

  m_hash = qHash(tListUrl[v].name);

  ui->pushButton->setEnabled(false);

  QNetworkRequest request(url);
  manager = new QNetworkAccessManager(this);
  manager->get(request);

  connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(slotDownFinished(QNetworkReply*)));

  ui->pushButton_2->setDisabled(true);
  ui->pushButton->setDisabled(true);
  ui->comboBox->setDisabled(true);

  ui->widget->setSourceWaiting();

  setCursor(Qt::WaitCursor);
}


/////////////////////////////////////////////////////
void CSohoImg::slotDownFinished(QNetworkReply *reply)
/////////////////////////////////////////////////////
{
  if (reply->error())
  {
    msgBoxError(this, QString("Download failed (%1)").arg(reply->errorString()));
    ui->widget->setSourceNoImage();
  }
  else
  {
    QImage img;
    QPixmap pix;

    img = QImage::fromData(reply->readAll());
    img.save(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/sun/sun_" + QString("%1").arg(m_hash, 0, 16) + ".jpg");

    if (img.isNull())
    {
      ui->widget->setSourceNoImage();
    }
    else
    {
      pix.convertFromImage(img);
      ui->widget->setSource(&pix);
    }
  }

  ui->pushButton_2->setEnabled(true);
  ui->pushButton->setEnabled(true);
  ui->comboBox->setEnabled(true);

  setCursor(Qt::ArrowCursor);

  reply->deleteLater();
  manager->deleteLater();
}


//////////////////////////////////////////
void CSohoImg::keyPressEvent(QKeyEvent *e)
//////////////////////////////////////////
{
  if(e->key() != Qt::Key_Escape)
    QDialog::keyPressEvent(e);
}


/////////////////////////////////////////////////////////
void CSohoImg::on_comboBox_currentIndexChanged(int index)
/////////////////////////////////////////////////////////
{
  m_hash = qHash(tListUrl[index].name);

  QPixmap pix(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/sun/sun_" + QString("%1").arg(m_hash, 0, 16) + ".jpg");

  if (pix.isNull())
  {
    ui->widget->setSourceNoImage();
    return;
  }

  ui->widget->setSource(&pix);
}


//////////////////////////////////////
void CSohoImg::on_pushButton_clicked()
//////////////////////////////////////
{
  done(DL_OK);
}

////////////////////////////////////////
void CSohoImg::on_pushButton_3_clicked()
////////////////////////////////////////
{
  if (createSunTexture(ui->widget->getSource()))
  {
    msgBoxInfo(this, tr("Using this image as Sun texture!"));
  }
  else
  {
    msgBoxError(this, "Image error!!!");
  }
}

////////////////////////////////////////
void CSohoImg::on_pushButton_4_clicked()
////////////////////////////////////////
{
  removeSunTexture();

  msgBoxInfo(this, tr("Default Sun texture is used!"));
}
