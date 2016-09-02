#include "cimagemanip.h"
#include "skcore.h"
#include "skutils.h"

static int contrastTable[256];
static int gammaTable[256];
static int autoTable[256];

static inline void createContrastsTable(float contrast, int *table)
{
  for (int i = 0; i < 256; i++)
  {
    table[i] = CLAMP((i - 127) * contrast + 127, 0, 255);
  }
}

static inline void createGammasTable(float gamma, int *table)
{
  for (int i = 0; i < 256; i++)
  {
    table[i] = CLAMP((int)pow(i, gamma), 0, 255);
  }
}

static inline void createAutosTable(int minVal, float delta, int *table)
{
  for (int i = 0; i < 256; i++)
  {
    table[i] = CLAMP((i - minVal) * delta, 0, 255);
  }
}

//////////////////////////
CImageManip::CImageManip()
//////////////////////////
{
}

//////////////////////////////////////////////////////////////////////
void CImageManip::process(const QImage *src, QImage *dst, imageParam_t *par)
//////////////////////////////////////////////////////////////////////
{
  bool bw = src->format() == QImage::Format_Indexed8;

  if (par->autoAdjust)
  {
    autoAdjust(src, dst, par);
    return;
  }

  float con = (par->contrast / 100.0);
  float gamma = par->gamma / 150.0;

  int val;
  QRgb rgb;
  int x, y;

  createContrastsTable(con, contrastTable);
  createGammasTable(gamma, gammaTable);

  if (bw)
  {
    const uchar *s = (uchar *)src->bits();
    uchar *d = (uchar *)dst->bits();

    #pragma omp parallel for shared(s, d, src, con, gamma, par) private (x, y, val)
    for (y = 0; y < src->height(); y++)
    {
      int index = src->width() * y;
      for (x = 0; x < src->width(); x++)
      {
        val = s[x + index];

        val += par->brightness;
        val = CLAMP(val, 0, 255);

        val = contrastTable[val];
        val = gammaTable[val];

        val = par->invert ? 255 - val : val;

        d[x + index] = val;
      }
    }
  }
  else
  {
    const QRgb *s = (QRgb *)src->bits();
    QRgb *d = (QRgb *)dst->bits();

    #pragma omp parallel for shared(s, d, src, con, gamma, par) private (x, y, val, rgb)
    for (y = 0; y < src->height(); y++)
    {
      int index = src->width() * y;
      for (x = 0; x < src->width(); x++)
      {
        rgb = s[x + index];
        val = rgb & 0xff;

        val += par->brightness;
        val = CLAMP(val, 0, 255);

        val = contrastTable[val];
        val = gammaTable[val];

        val = par->invert ? 255 - val : val;

        d[x + index] = (255 << 24) | (val << 16) | (val << 8) | val;
      }
    }
  }

  if (par->useMatrix)
  {
    double summ = 0;

    for (int fy = 0; fy < 3; fy++)
    {
      for (int fx = 0; fx < 3; fx++)
      {
        summ += par->matrix[fy][fx];
      }
    }

    #define OFFSET(x, y) ((x) + ((y) * src->width()))

    if (bw)
    {
      QImage tmp(*dst);

      const uchar *s = (uchar *)tmp.bits();
      uchar *d = (uchar *)dst->bits();

      #pragma omp parallel for shared(s, d, par) private (x, y, val)
      for (y = 1; y < src->height() - 1; y++)
      {
        int index = src->width() * y;
        for (x = 1; x < src->width() - 1; x++)
        {
          int total = 0;
          for (int fy = -1; fy <= 1; fy++)
          {
            for (int fx = -1; fx <= 1; fx++)
            {
              val = s[OFFSET(x + fx, y + fy)];
              total += val * par->matrix[fy + 1][fx + 1];
            }
          }
          d[x + index] = val = CLAMP(total / summ, 0, 255);
        }
      }
    }
  }  
}

/////////////////////////////////////////////////////////////////
void CImageManip::getHistogram(const QImage *src, int *histogram)
/////////////////////////////////////////////////////////////////
{  
  bool bw = src->format() == QImage::Format_Indexed8;

  memset(histogram, 0, sizeof(int) * 256);  

  if (bw)
  {
    const uchar *p = (uchar *)src->bits();

    for (int i = 0; i < src->width() * src->height(); i++, p++)
    {
      int val = *p;

      histogram[val]++;
    }
  }
  else
  {
    const QRgb *p = (QRgb *)src->bits();
    for (int i = 0; i < src->width() * src->height(); i++, p++)
    {
      QRgb rgb = *p;
      int val = rgb & 0xff;

      histogram[val]++;
    }
  }
}

////////////////////////////////////////////////////////////////////
void CImageManip::getMinMax(const QImage *src, int &minv, int &maxv)
////////////////////////////////////////////////////////////////////
{
  bool bw = src->format() == QImage::Format_Indexed8;
  minv = 255;
  maxv = 0;

  int histogram[256];

  memset(&histogram, 0, sizeof(histogram));

  if (bw)
  {
    const uchar *p = (uchar *)src->bits();

    for (int i = 0; i < src->width() * src->height(); i++, p++)
    {
      int val = *p;

      histogram[val]++;

      if (val < minv)
        minv = val;
    }
  }
  else
  {
    const QRgb *p = (QRgb *)src->bits();
    for (int i = 0; i < src->width() * src->height(); i++, p++)
    {
      QRgb rgb = *p;
      int val = rgb & 0xff;

      histogram[val]++;

      if (val < minv)
        minv = val;
    }
  }

  // TODO: udelat to inteligentne (to minimum)
  int counter = 0;
  for (int i = 255; i >= 0; i--)
  {
    counter += histogram[i];
    if (counter > 16)
    {
      maxv = i;
      break;
    }
  }
}

/////////////////////////////////////////////////////////////////////////
void CImageManip::autoAdjust(const QImage *src, QImage *dst, imageParam_t *par)
/////////////////////////////////////////////////////////////////////////
{
  bool bw = src->format() == QImage::Format_Indexed8;
  int maxv;
  int minv;

  getMinMax(src, minv, maxv);

  int val;
  QRgb rgb;
  int x, y;

  SK_DEBUG_TIMER_START(0);

  float delta = 256 / (float)(maxv - minv);

  createAutosTable(minv, delta, autoTable);

  if (bw)
  {
    const uchar *s = (uchar *)src->bits();

    uchar *d = (uchar *)dst->bits();

    #pragma omp parallel for shared(s, d, src, par, delta) private (x, y, val)
    for (y = 0; y < src->height(); y++)
    {
      int index = src->width() * y;
      for (x = 0; x < src->width(); x++)
      {
        val = s[x + index];

        val = autoTable[val];

        if (par->invert)
          val = 255 - val;

        d[x + index] = val;
      }
    }
  }
  else
  {
    const QRgb *s = (QRgb *)src->bits();
    QRgb *d = (QRgb *)dst->bits();

    #pragma omp parallel for shared(s, d, src, par, delta) private (x, y, val, rgb)
    for (y = 0; y < src->height(); y++)
    {
      int index = src->width() * y;
      for (x = 0; x < src->width(); x++)
      {
        rgb = s[x + index];
        val = rgb & 0xff;

        val = autoTable[val];

        if (par->invert)
          val = 255 - val;

        d[x + index] = (255 << 24) | (val << 16) | (val << 8) | val;
      }
    }
  }

  SK_DEBUG_TIMER_STOP(0);
}

