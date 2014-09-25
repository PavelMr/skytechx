#ifndef CSTARRENDERER_H
#define CSTARRENDERER_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"

class CStarRenderer
{
public:
    CStarRenderer();
   ~CStarRenderer();
    bool open(QString filename);
    int  renderStar(SKPOINT *pt, int spt, float mag, QPainter *p);
    int  getStarSize(float mag);
    int  getStarRadius(float mag);
    void setMaxMag(float mMag);
    QPixmap getExampleStar(void);
    QPixmap getExampleStars();

protected:
    QPixmap *pImg;
    float    maxMag;
    int     *pRadius;
    int      numStars;
    int      starSize;
};

extern CStarRenderer   cStarRenderer;

#endif // CSTARRENDERER_H
