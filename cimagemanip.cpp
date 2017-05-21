#include "cimagemanip.h"
#include "skcore.h"
#include "skutils.h"

static int contrastTable[256];
static int gammaTable[256];
static int autoTable[256];
static int autoTableR[256];
static int autoTableG[256];
static int autoTableB[256];

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
  bool bw = src->format() == (QImage::Format_Indexed8) || (src->format() == QImage::Format_Grayscale8);

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
    int valRGB[3];

    #pragma omp parallel for shared(s, d, src, con, gamma, par) private (x, y, valRGB, rgb)
    for (y = 0; y < src->height(); y++)
    {
      int index = src->width() * y;
      for (x = 0; x < src->width(); x++)
      {
        rgb = s[x + index];
        valRGB[0] = qRed(rgb);
        valRGB[1] = qGreen(rgb);
        valRGB[2] = qBlue(rgb);

        for (int i = 0; i < 3; i++)
        {
          valRGB[i] += par->brightness;
          valRGB[i] = CLAMP(valRGB[i], 0, 255);

          valRGB[i] = contrastTable[valRGB[i]];
          valRGB[i] = gammaTable[valRGB[i]];

          valRGB[i] = par->invert ? 255 - valRGB[i] : valRGB[i];
        }

        d[x + index] = qRgb(valRGB[0], valRGB[1], valRGB[2]);
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
    } else // COLOR
    {
      QImage tmp(*dst);

      const QRgb *s = (QRgb *)tmp.bits();
      QRgb *d = (QRgb *)dst->bits();

      #pragma omp parallel for shared(s, d, par) private (x, y)
      for (y = 1; y < src->height() - 1; y++)
      {
        int index = src->width() * y;
        for (x = 1; x < src->width() - 1; x++)
        {
          int valR, valG, valB;
          int totalR = 0;
          int totalG = 0;
          int totalB = 0;
          for (int fy = -1; fy <= 1; fy++)
          {
            for (int fx = -1; fx <= 1; fx++)
            {
              valR = qRed(s[OFFSET(x + fx, y + fy)]);
              valG = qGreen(s[OFFSET(x + fx, y + fy)]);
              valB = qBlue(s[OFFSET(x + fx, y + fy)]);

              totalR += valR * par->matrix[fy + 1][fx + 1];
              totalG += valG * par->matrix[fy + 1][fx + 1];
              totalB += valB * par->matrix[fy + 1][fx + 1];
            }
          }
          valR = CLAMP(totalR / summ, 0, 255);
          valG = CLAMP(totalG / summ, 0, 255);
          valB = CLAMP(totalB / summ, 0, 255);

          d[x + index] = qRgb(valR, valG, valB);
        }
      }
    }
  }  
}

/////////////////////////////////////////////////////////////////
void CImageManip::getHistogram(const QImage *src, int *histogram)
/////////////////////////////////////////////////////////////////
{  
  bool bw = src->format() == (QImage::Format_Indexed8) || (src->format() == QImage::Format_Grayscale8);

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
void CImageManip::getMinMax(const QImage *src, int &minv, int &maxv, int comp)
////////////////////////////////////////////////////////////////////
{
  bool bw = src->format() == (QImage::Format_Indexed8) || (src->format() == QImage::Format_Grayscale8);
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
      int val;

      switch (comp)
      {
        case 0:
          val = rgb & 0xff;
          break;

        case 1:
          val = (rgb & 0xff00) >> 8;
          break;

        case 2:
          val = (rgb & 0xff0000) >> 16;
          break;
      }

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
  bool bw = src->format() == (QImage::Format_Indexed8) || (src->format() == QImage::Format_Grayscale8);
  int maxv;
  int minv;
  int maxa[3];
  int mina[3];

  if (bw)
  {
    getMinMax(src, minv, maxv);
  }
  else
  {
    getMinMax(src, mina[0], maxa[0], 0);
    getMinMax(src, mina[1], maxa[1], 1);
    getMinMax(src, mina[2], maxa[2], 2);
  }

  int val;
  QRgb rgb;
  int x, y;

  SK_DEBUG_TIMER_START(0);  

  if (bw)
  {
    float delta = 256 / (float)(maxv - minv);
    createAutosTable(minv, delta, autoTable);

    const uchar *s = (uchar *)src->bits();

    uchar *d = (uchar *)dst->bits();

    #pragma omp parallel for shared(s, d, src, par) private (x, y, val)
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
    float deltaR = 256 / (float)(maxa[0] - mina[0]);
    float deltaG = 256 / (float)(maxa[1] - mina[1]);
    float deltaB = 256 / (float)(maxa[2] - mina[2]);

    createAutosTable(mina[0], deltaR, autoTableR);
    createAutosTable(mina[1], deltaG, autoTableG);
    createAutosTable(mina[2], deltaB, autoTableB);

    const QRgb *s = (QRgb *)src->bits();
    QRgb *d = (QRgb *)dst->bits();

    #pragma omp parallel for shared(s, d, src, par) private (x, y, rgb)
    for (y = 0; y < src->height(); y++)
    {
      int index = src->width() * y;
      for (x = 0; x < src->width(); x++)
      {
        rgb = s[x + index];
        int valR = qRed(rgb);
        int valG = qGreen(rgb);
        int valB = qBlue(rgb);

        valR = autoTableR[valR];
        valG = autoTableR[valG];
        valB = autoTableR[valB];

        if (par->invert)
        {
          valR = 255 - valR;
          valG = 255 - valG;
          valB = 255 - valB;
        }

        d[x + index] = qRgb(valR, valG, valB);
      }
    }
  }

  SK_DEBUG_TIMER_STOP(0);
}

