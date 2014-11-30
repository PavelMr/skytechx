#include "skcore.h"
#include "transform.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "cscanrender.h"
#include "castro.h"
#include "background.h"
#include "setting.h"

typedef struct
{
  double  azm;
  QString name;
} bkNames_t;

static double             altHorizon[360];
static QList <bkNames_t>  bkNames;

extern QString g_horizonName;

// TODO: nezobrazuje se spravne pri obs lat 90 a -90

//////////////////////////
void resetBackground(void)
//////////////////////////
{
  memset(altHorizon, 0, sizeof(altHorizon));
  bkNames.clear();
}

/////////////////////////////////
bool loadBackground(QString name)
/////////////////////////////////
{
  QList <QPointF> pts;
  QFile f(name);

  resetBackground();

  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QTextStream s(&f);

    QString str = "";
    while (!str.isNull())
    {
      str = s.readLine();

      QStringList list;

      if (str.isEmpty())
        continue;

      if (str.startsWith("#")) // comment
        continue;

      list = str.split(',');

      if (list.count() == 2)
      {
        if (list.at(0).at(0) == '@')
        { // names
          bkNames_t n;

          n.azm = list.at(0).mid(1).toDouble();
          n.name = list.at(1);

          bkNames.append(n);
          continue;
        }

        QPoint p(list.at(0).toInt(), list.at(1).toFloat());

        pts.append(p);
      }
    }
  }
  else
  {
    return false;
  }

  if (!makeHorizon(&pts))
  {
    resetBackground();
    return false;
  }

  return true;
}

///////////////////////////////////////
bool makeHorizon(QList <QPointF> *list)
///////////////////////////////////////
{
  if (list->count() < 3)
    return(false);

  for (int d = 0; d < 360; d++)
  {
    double dmin = list->at(0).x();
    double dmax = list->at(0).x();
    double amin = list->at(0).y();
    double amax = list->at(0).y();

    for (int i = 0; i < list->count(); i++)
    {
      if (d >= list->at(i).x())
      {
        dmin = list->at(i).x();
        amin = list->at(i).y();

        if (i + 1 < list->count())
        {
          dmax = list->at(i + 1).x();
          amax = list->at(i + 1).y();
        }
        else
        {
          dmax = 360;
          amax = list->at(0).y();
        }
      }
    }

    double alpha;

    if (dmax == dmin)
      alpha = (d - dmin);
    else
      alpha = (d - dmin) / (dmax - dmin);
    double h = (1.0 - alpha) * amin + alpha * amax;

    altHorizon[d] = D2R(h);
  }

  return(true);
}

/////////////////////////////////////////////////////////////////////
void renderHorizonBk(mapView_t *mapView, CSkPainter *p, QImage *pImg)
/////////////////////////////////////////////////////////////////////
{
  CScanRender scan;
  SKPOINT     newPts[MAX_POLYGON_PTS];
  int         newCount;
  QColor      color = g_skSet.map.hor.color;

  if (!g_horizonName.compare("none"))
  {
    return;
  }

  color.setAlpha(g_skSet.map.hor.alpha);

  for (int d = 0; d < 360; d++)
  {
    int     d1 = (d + 1) % 360;
    radec_t aa[3];
    SKPOINT pt[3];

    aa[0].Ra = D2R(d);
    aa[0].Dec = altHorizon[d];

    aa[1].Ra = D2R(d + 1);
    aa[1].Dec = altHorizon[d1];

    aa[2].Ra = 0;
    aa[2].Dec = D2R(-90);

    for (int i = 0; i < 3; i++)
    {
      radec_t rd;

      cAstro.convAA2RDRef(aa[i].Ra, aa[i].Dec, &rd.Ra, &rd.Dec);
      trfRaDecToPointCorrectFromTo(&rd, &pt[i], mapView->jd, JD2000);
    }

    if (SKPLANEClipPolygonToFrustum(trfGetFrustum(), pt, 3, newPts, newCount))
    {
      scanRender.resetScanPoly(pImg->width(), pImg->height());

      for (int t = 0; t < newCount; t++)
        trfProjectPointNoCheck(&newPts[t]);

      for (int t = 0; t < newCount; t++)
      {
        int t1 = (t + 1) % newCount;
        scanRender.scanLine(newPts[t].sx, newPts[t].sy, newPts[t1].sx, newPts[t1].sy);
      }

      if (color.alpha() == 255)
        scanRender.renderPolygon(color, pImg);
      else
        scanRender.renderPolygonAlpha(color, pImg);
    }
  }

  auto drawText = [color](SKPOINT* pt1, SKPOINT* pt2, const QString& text, CSkPainter* p)
  {
    if (!SKPLANECheckFrustumToSphere(trfGetFrustum(), &pt2->w, D2R(5)))
    {
      return;
    }

    QRect rc;
    QRect rc2;
    QFontMetrics fm(p->font());

    trfProjectPointNoCheck(pt1);
    trfProjectPointNoCheck(pt2);

    rc = fm.boundingRect(text);
    rc.moveCenter(QPoint(pt2->sx, pt2->sy));
    rc2 = rc;

    rc2.adjust(-4, -4, 4, 4);

    p->setPen(QPen(color, 2));
    p->drawLine(pt1->sx, pt1->sy, pt2->sx, pt2->sy);

    p->setBrush(color);
    setSetFontColor(FONT_HORIZON, p);
    p->drawRoundedRect(rc2, 5, 5);

    setSetFontColor(FONT_HORIZON, p);
    p->drawText(rc, Qt::AlignCenter, text);
  };

  int azm[] = {0, 45, 90, 135, 180, 225, 270, 315};
  QString azmText[] = {QObject::tr("N"), QObject::tr("NE"), QObject::tr("E"), QObject::tr("SE"), QObject::tr("S"), QObject::tr("SW"), QObject::tr("W"), QObject::tr("NW")};
  for (int i = 0; i < 8; i++)
  {
    radec_t aa;
    radec_t rd;
    SKPOINT pt1;
    SKPOINT pt2;
    double btm = altHorizon[(int)azm[i]];

    aa.Ra = D2R(azm[i]);
    aa.Dec = btm;
    cAstro.convAA2RDRef(aa.Ra, aa.Dec, &rd.Ra, &rd.Dec);
    trfRaDecToPointCorrectFromTo(&rd, &pt1, mapView->jd, JD2000);

    aa.Dec = btm + D2R(1.5);
    cAstro.convAA2RDRef(aa.Ra, aa.Dec, &rd.Ra, &rd.Dec);
    trfRaDecToPointCorrectFromTo(&rd, &pt2, mapView->jd, JD2000);

    drawText(&pt1, &pt2, azmText[i], p);
  }

  for (int i = 0; i < bkNames.count(); i++)
  {
    radec_t aa;
    radec_t rd;
    SKPOINT pt1;
    SKPOINT pt2;
    double btm = altHorizon[(int)bkNames[i].azm];

    aa.Ra = D2R(bkNames[i].azm);
    aa.Dec = btm;
    cAstro.convAA2RDRef(aa.Ra, aa.Dec, &rd.Ra, &rd.Dec);
    trfRaDecToPointCorrectFromTo(&rd, &pt1, mapView->jd, JD2000);

    aa.Ra = D2R(bkNames[i].azm);
    aa.Dec = btm + D2R(3);
    cAstro.convAA2RDRef(aa.Ra, aa.Dec, &rd.Ra, &rd.Dec);
    trfRaDecToPointCorrectFromTo(&rd, &pt2, mapView->jd, JD2000);

    drawText(&pt1, &pt2, bkNames[i].name, p);
  }

}

