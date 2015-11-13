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
    void setMaxMag(float mMag);
    QPixmap getExampleStar(void);

    static uchar getSPIndex(float bvIndex);

protected:
    QList <QPixmap> pStars[8];
    float    maxMag;
    int      numStars;
    int      starSize;
};

extern CStarRenderer   cStarRenderer;

#endif // CSTARRENDERER_H
