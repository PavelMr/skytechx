#include "cimagemanip.h"
#include "skcore.h"

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
void CImageManip::process(QImage *src, QImage *dst, imageParam_t *par)
//////////////////////////////////////////////////////////////////////
{
  if (par->autoAdjust)
  {
    autoAdjust(src, dst, par);
    return;
  }

  QRgb *s = (QRgb *)src->bits();
  QRgb *d = (QRgb *)dst->bits();

  float con = (par->contrast / 100.0);
  float gamma = par->gamma / 150.0;

  int val;
  QRgb rgb;
  int x, y;

  createContrastsTable(con, contrastTable);
  createGammasTable(gamma, gammaTable);

  #pragma omp parallel for shared(s, d, src, con, gamma, par) private (x, y, val, rgb)
  for (y = 0; y < src->height(); y++)
  {
    int bw = src->width() * y;
    for (x = 0; x < src->width(); x++)
    {
      rgb = s[x + bw];
      val = rgb & 0xff;

      val += par->brightness;
      val = CLAMP(val, 0, 255);

      val = contrastTable[val];
      val = gammaTable[val];

      val = par->invert ? 255 - val : val;

      d[x + bw] = (255 << 24) | (val << 16) | (val << 8) | val;
    }
  }
}

//////////////////////////////////////////////////////////////
void CImageManip::getMinMax(QImage *src, int &minv, int &maxv)
//////////////////////////////////////////////////////////////
{
  minv = 255;
  maxv = 0;

  int histogram[256];

  memset(&histogram, 0, sizeof(histogram));

  QRgb *p = (QRgb *)src->bits();

  for (int i = 0; i < src->width() * src->height(); i++, p++)
  {
    QRgb rgb = *p;
    int val = rgb & 0xff;

    histogram[val]++;

    if (val < minv)
      minv = val;
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
void CImageManip::autoAdjust(QImage *src, QImage *dst, imageParam_t *par)
/////////////////////////////////////////////////////////////////////////
{
  int maxv;
  int minv;

  getMinMax(src, minv, maxv);

  QRgb *s = (QRgb *)src->bits();
  QRgb *d = (QRgb *)dst->bits();

  int val;
  QRgb rgb;
  int x, y;

  SK_DEBUG_TIMER_START(0);

  float delta = 256 / (float)(maxv - minv);

  createAutosTable(minv, delta, autoTable);

  #pragma omp parallel for shared(s, d, src, par, delta) private (x, y, val, rgb)
  for (y = 0; y < src->height(); y++)
  {
    int bw = src->width() * y;
    for (x = 0; x < src->width(); x++)
    {
      rgb = s[x + bw];
      val = rgb & 0xff;

      val = autoTable[val];

      if (par->invert)
        val = 255 - val;

      d[x + bw] = (255 << 24) | (val << 16) | (val << 8) | val;
    }
  }

  SK_DEBUG_TIMER_STOP(0);
}

