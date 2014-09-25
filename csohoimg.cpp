#include "csohoimg.h"
#include "ui_csohoimg.h"

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

  CUrlFile::readFile("data/urls/sun.url", &tListUrl);

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
    ui->pushButton_3->setEnabled(false);
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
  ui->pushButton_3->setDisabled(true);
  ui->pushButton_4->setDisabled(true);
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
    img.save("data/sun/" + QString("%1").arg(m_hash, 0, 16) + ".jpg");

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
  ui->pushButton_3->setEnabled(true);
  ui->pushButton_4->setEnabled(true);
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

  QPixmap pix("data/sun/" + QString("%1").arg(m_hash, 0, 16) + ".jpg");

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
  if (g_pSunTexture)
  {
    delete g_pSunTexture;
    g_pSunTexture = NULL;
  }

  QPixmap *pix = ui->widget->getSource();

  if (pix != NULL)
  {
    if (!pix->isNull())
    {
      QImage tmp;

      tmp = pix->toImage();

      int m = -1;

      for (int y = 0; y < tmp.height(); y++)
      {
        QColor c = tmp.pixel(tmp.width() / 2, y);

        if (c.toHsl().lightness() > 20)
        {
          m = y;
          break;
        }
      }

      if (m != -1)
      {
        QImage crop = tmp.copy(m, m, tmp.width() - m * 2, tmp.width() - m * 2);
        QImage fmt = crop.convertToFormat(QImage::Format_ARGB32);

        g_pSunTexture = new QImage(fmt);

        for (int y = 0; y < g_pSunTexture->height(); y++)
        {
          QRgb *row = (QRgb*)g_pSunTexture->scanLine(y);
          for (int x = 0; x < g_pSunTexture->width(); x++)
          {
            if (QColor(row[x]).toHsl().lightness() < 15)
              ((unsigned char*)&row[x])[3] = 0;
          }
        }
        g_pSunTexture->save("data/sun/sun_tex.png", "PNG");
      }
      else
      {
        msgBoxError(this, "Image error!!!");
        return;
      }
    }
  }
  msgBoxInfo(this, tr("Using this image as Sun texture!"));
}

////////////////////////////////////////
void CSohoImg::on_pushButton_4_clicked()
////////////////////////////////////////
{
  if (g_pSunTexture)
  {
    delete g_pSunTexture;
    g_pSunTexture = NULL;
  }

  msgBoxInfo(this, tr("Default Sun texture is used!"));
}
