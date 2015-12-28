#ifndef CSTARRENDERER_H
#define CSTARRENDERER_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "csetting.h"

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
    void setConfig(setting_t *set); // call before open

    static uchar getSPIndex(float bvIndex);

protected:
    QList <QPixmap> pStars[8];
    QList <QPixmap> pStarsOrig[8];
    float    maxMag;
    int      numStars;
    int      starSize;
    double   m_starSizeFactor;
    int      m_saturation;
    bool     m_useSpectralTp;
};

extern CStarRenderer   cStarRenderer;

#endif // CSTARRENDERER_H
