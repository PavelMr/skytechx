#include "suntexture.h"
#include "jd.h"
#include "cstatusbar.h"
#include "skutils.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QSettings>
#include <QPainter>

extern CStatusBar *g_statusBar;
extern QImage *g_pSunTexture;

SunOnlineDaemon g_sunOnlineDaemon;

static bool sunOnlineRunnning = false;

void removeSunTexture()
{
  if (g_pSunTexture)
  {
    delete g_pSunTexture;
    g_pSunTexture = NULL;
  }
}

bool createSunTexture(const QPixmap *pix, int radius)
{
  if (g_pSunTexture)
  {
    delete g_pSunTexture;
    g_pSunTexture = NULL;
  }

  if (pix != NULL)
  {
    if (!pix->isNull())
    {
      if (radius == 0)
      { // automatic
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
          g_pSunTexture->save(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/sun/sun_tex.png", "PNG");
        }
        else
        {
          return false;
        }
      }
      else
      {
        QImage tmp;

        int margin = (pix->width() / 2) - radius;

        tmp = pix->toImage();
        QImage tmp2 = tmp.copy(margin, margin, radius * 2, radius * 2);

        QImage fmt = tmp2.convertToFormat(QImage::Format_ARGB32_Premultiplied);

        g_pSunTexture = new QImage(fmt);
        QPainter p;
        QPainterPath path;
        QPainterPath rect;

        rect.addRect(0, 0, g_pSunTexture->width(), g_pSunTexture->height());
        path.addEllipse(QPoint(g_pSunTexture->width() / 2, g_pSunTexture->height() / 2), radius, radius);

        path = path.subtractedInverted(rect);

        p.begin(g_pSunTexture);
        p.setClipPath(path);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(QRect(0, 0, g_pSunTexture->width(), g_pSunTexture->height()), Qt::transparent);
        p.end();

        g_pSunTexture->save(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/sun/sun_tex.png", "PNG");
      }
    }
  }

  return true;
}

void SunOnline::start(const QString &url, int radius)
{
  qDebug() << "start" << url << radius;
  if (sunOnlineRunnning)
  {
    qDebug() << "running";
    deleteLater();
    return;
  }

  QNetworkRequest request(url);

  m_radius = radius;
  m_manager.get(request);

  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(slotDownFinished(QNetworkReply*)));

  g_statusBar->setDownloadStatus(true);
  sunOnlineRunnning = true;
}

void SunOnline::slotDownFinished(QNetworkReply *reply)
{
  if (reply->error())
  {
    qDebug() << "error";
  }
  else
  {
    QImage img;
    QPixmap pix;
    QByteArray data = reply->readAll();

    img = QImage::fromData(data);

    qDebug() << img;
    if (img.isNull())
    {
      msgBoxError(nullptr, data);
    }    

    pix = pix.fromImage(img);

    createSunTexture(&pix, m_radius);

    qDebug() << "sun done";

    emit done();
  }

  reply->deleteLater();
  deleteLater();
  sunOnlineRunnning = false;
  g_statusBar->setDownloadStatus(false);
}


SunOnlineDaemon::SunOnlineDaemon()
{
}

void SunOnlineDaemon::setupParams()
{
  QSettings set;

  m_used = set.value("sun_online_used", false).toBool();
  m_radius = set.value("sun_radius", 0).toInt();
  m_url = set.value("sun_online_url", "").toString();
  m_period = set.value("sun_online_period", 0).toInt();
  m_lastJD = set.value("sun_online_last_update", 0.0).toDouble();
  m_startupOnly = set.value("sun_online_startup_only", true).toBool();
}

void SunOnlineDaemon::start()
{
  g_statusBar->setDownloadStatus(false, true);

  if (m_used)
  {
    m_firstTime = true;
    timer();
  }
  else
  {
    if (g_pSunTexture)
    {
      delete g_pSunTexture;
    }
    g_pSunTexture = NULL;
    emit repaint();
  }
}

void SunOnlineDaemon::stop()
{
  m_timer.stop();
}

void SunOnlineDaemon::timer()
{
  //qDebug() << "sun daemon timer" << m_period;
  if (m_firstTime || m_startupOnly || (jdGetCurrentJD() > (m_lastJD + (JD1SEC * m_period))))
  {
    SunOnline *online = new SunOnline;
    online->start(m_url, m_radius);

    qDebug() << "sun daemon download";

    connect(online, SIGNAL(done()), this, SLOT(done()));

    m_lastJD = jdGetCurrentJD();

    QSettings set;
    set.setValue("sun_online_last_update", m_lastJD);
  }

  if (!m_startupOnly)
  {
    m_timer.singleShot(60000, this, SLOT(timer()));
    //qDebug() << "sun daemon set timer";
  }

  m_firstTime = false;
}

void SunOnlineDaemon::done()
{
  qDebug() << "sun daemon repaint";
  emit repaint();
}
