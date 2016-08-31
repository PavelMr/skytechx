#ifndef CIMAGEMANIP_H
#define CIMAGEMANIP_H

#include "cbkimages.h"

class CImageManip
{
  public:
    CImageManip();
    static void process(const QImage *src, QImage *dst, imageParam_t *par);
    static void getMinMax(const QImage *src, int &minv, int &maxv);
    static void autoAdjust(const QImage *src, QImage *dst, imageParam_t *par);
    static void getHistogram(const QImage *src, int *histogram);
};

#endif // CIMAGEMANIP_H
