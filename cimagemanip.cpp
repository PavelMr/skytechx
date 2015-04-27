#include "cimagemanip.h"
#include "skcore.h"

#include <cuda_runtime.h>
#include <cuda.h>

extern "C" void cuAutoAdjust(unsigned char *s, unsigned char *d, int count, int *autoTable, bool invert);
extern "C" void cuProcessImage(unsigned char *s, unsigned char *d, int count, int *contrastTable, int *gammaTable, bool invert, float brightness);

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

  if (0)
  {
    if (bw)
    {
      uchar *s = (uchar *)src->bits();
      uchar *d = (uchar *)dst->bits();

      #pragma omp parallel for shared(s, d, src, con, gamma, par) private (x, y, val, rgb)
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
      QRgb *s = (QRgb *)src->bits();
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
  }
  else
  { // CUDA
    bool bw = src->format() == QImage::Format_Indexed8;
    int maxv;
    int minv;

    getMinMax(src, minv, maxv);

    SK_DEBUG_TIMER_START(0);

    float delta = 256 / (float)(maxv - minv);

    createAutosTable(minv, delta, autoTable);

    if (bw)
    {
      uchar *s = (uchar *)src->bits();
      uchar *d = (uchar *)dst->bits();
      int *contrastTable_d;
      int *gammaTable_d;
      int count = src->byteCount();

      uchar *s_d = (uchar *)src->bits();
      uchar *d_d = (uchar *)dst->bits();

      cudaMalloc(&contrastTable_d, 256 * sizeof(int));
      cudaMalloc(&gammaTable_d, 256 * sizeof(int));
      cudaMalloc(&s_d, count);
      cudaMalloc(&d_d, count);

      cudaMemcpy(s_d, s, count, cudaMemcpyHostToDevice);
      cudaMemcpy(contrastTable_d, contrastTable, 256 * sizeof(int), cudaMemcpyHostToDevice);
      cudaMemcpy(gammaTable_d, gammaTable, 256 * sizeof(int), cudaMemcpyHostToDevice);

      cuProcessImage(s_d, d_d, count, contrastTable_d, gammaTable_d, par->invert, par->brightness);

      cudaMemcpy(d, d_d, count, cudaMemcpyDeviceToHost);

      cudaFree(s_d);
      cudaFree(d_d);
      cudaFree(contrastTable_d);
      cudaFree(gammaTable_d);
    }
    else
    {

    }

    SK_DEBUG_TIMER_STOP(0);
  }

}

//////////////////////////////////////////////////////////////
void CImageManip::getMinMax(QImage *src, int &minv, int &maxv)
//////////////////////////////////////////////////////////////
{
  bool bw = src->format() == QImage::Format_Indexed8;
  minv = 255;
  maxv = 0;

  int histogram[256];

  memset(&histogram, 0, sizeof(histogram));

  if (bw)
  {
    uchar *p = (uchar *)src->bits();
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
    QRgb *p = (QRgb *)src->bits();
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
void CImageManip::autoAdjust(QImage *src, QImage *dst, imageParam_t *par)
/////////////////////////////////////////////////////////////////////////
{

  if (0)
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
      uchar *s = (uchar *)src->bits();
      uchar *d = (uchar *)dst->bits();

      #pragma omp parallel for shared(s, d, src, par, delta) private (x, y, val, rgb)
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
      QRgb *s = (QRgb *)src->bits();
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
  else
  { // CUDA
    bool bw = src->format() == QImage::Format_Indexed8;
    int maxv;
    int minv;

    getMinMax(src, minv, maxv);

    SK_DEBUG_TIMER_START(0);

    float delta = 256 / (float)(maxv - minv);

    createAutosTable(minv, delta, autoTable);

    if (bw)
    {
      uchar *s = (uchar *)src->bits();
      uchar *d = (uchar *)dst->bits();
      int *autoTable_d;
      int count = src->byteCount();

      uchar *s_d = (uchar *)src->bits();
      uchar *d_d = (uchar *)dst->bits();

      cudaMalloc(&autoTable_d, 256 * sizeof(int));
      cudaMalloc(&s_d, count);
      cudaMalloc(&d_d, count);

      cudaMemcpy(s_d, s, count, cudaMemcpyHostToDevice);
      cudaMemcpy(autoTable_d, autoTable, 256 * sizeof(int), cudaMemcpyHostToDevice);

      cuAutoAdjust(s_d, d_d, count, autoTable_d, par->invert);

      cudaMemcpy(d, d_d, count, cudaMemcpyDeviceToHost);

      cudaFree(s_d);
      cudaFree(d_d);
      cudaFree(autoTable_d);
    }
    else
    {

    }

    SK_DEBUG_TIMER_STOP(0);
  }
}

