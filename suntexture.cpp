#include "suntexture.h"

extern QImage *g_pSunTexture;

void removeSunTexture()
{
  if (g_pSunTexture)
  {
    delete g_pSunTexture;
    g_pSunTexture = NULL;
  }
}

bool createSunTexture(const QPixmap *pix)
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
  }
  return true;
}
