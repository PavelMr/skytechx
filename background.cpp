#include "skcore.h"
#include "transform.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "cscanrender.h"
#include "castro.h"
#include "background.h"
#include "setting.h"
#include "cscanrender.h"

#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>

extern bool *g_bMouseMoveMap;
extern QString g_horizonName;

CBackground background;

// TODO: nezobrazuje se spravne pri obs lat 90 a -90


CBackground::CBackground()
{
  bkTexture = NULL;
}

CBackground::~CBackground()
{
  resetBackground();
}

void CBackground::resetBackground(void)
{
  if (bkTexture)
  {
    delete bkTexture;
    bkTexture = NULL;
  }

  memset(altHorizon, 0, sizeof(altHorizon));
  isValid = true;
  isTexture = false;

  altMap.clear();
  bkNames.clear();
}

bool CBackground::loadBackground(QString name)
{
  QList <QPointF> pts;
  QFile f(name);
  QScriptEngine eng;
  QString code;
  QFileInfo fi(name);

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

      if (str.startsWith("_"))
      {
        code += str;
        continue;
      }

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
        altMap[list.at(0).toInt()] = list.at(1).toFloat();
      }
    }
  }
  else
  {
    resetBackground();
    return false;
  }

  QScriptValue res = eng.evaluate(code);
  if (res.isError())
  {
    resetBackground();
    return false;
  }

  QScriptValueIterator it(eng.globalObject());
  while (it.hasNext())
  {
    it.next();

    if (!it.name().compare("_texture"))
    {
      QString name = fi.absolutePath() + "/" + it.value().toString();
      textureName = name;

      bkTexture = new QImage(name);

      if (bkTexture->isNull())
      {
        name = it.value().toString();
        bkTexture->load(name);
        textureName = name;
      }
    }
  }

  if (!makeHorizon(&pts, altHorizon))
  {
    if (!bkTexture || bkTexture->isNull())
    {
      resetBackground();
      return false;
    }
    isValid = false;
  }
  else
  {
    isValid = true;
  }

  if (bkTexture && !bkTexture->isNull())
  {
    isTexture = true;
  }

  //qDebug() << "tex" << textureName << isTexture;

  return true;
}

////////////////////////////////////////////////////
bool CBackground::makeHorizon(QList <QPointF> *list, double *alt)
////////////////////////////////////////////////////
{
  if (list->count() < 2)
    return(false);

  // TODO: kontrola vzestupu azimutu

  for (int i = 0; i < list->count(); i++)
  {
    int from = list->at(i).x();
    int to;
    double y1 = list->at(i).y();
    double y2;

    if (i + 1 >= list->count())
    {
      to = 360 + list->at(0).x();
      y2 = list->at(0).y();
    }
    else
    {
      to = list->at(i + 1).x();
      y2 = list->at(i + 1).y();
    }

    for (int a = from; a < to; a++)
    {
      int azm = a % 360;
      double f = FRAC(a, from, to);

      alt[azm] = D2R(LERP(f, y1, y2));
    }
  }

  return(true);
}

void CBackground::renderTexture(mapView_t *mapView, CSkPainter * /*p*/, QImage *pImg)
{
  SKMATRIX mat;
  SKMATRIX gmx, gmy;
  SKMATRIX precMat;

  precessMatrix(mapView->jd, JD2000, &precMat);

  SKMATRIXRotateX(&gmx, -cAstro.m_geoLat + R90);
  SKMATRIXRotateY(&gmy, -cAstro.m_lst + R180);

  mat = gmx * gmy * precMat;

  double tw = 1 / 36.0;
  double th = 1 / 18.0;
  double ox;
  double oy = 0;

  SKPOINT pt[4];
  radec_t rd[4];

  for (int y = 9; y > -9; y--)
  {
    ox = 1;
    for (int x = 0; x < 36; x++, ox -= tw)
    {
      rd[0].Ra = D2R(x * 10);
      rd[0].Dec = D2R(y * 10);

      rd[1].Ra = D2R(x * 10) - D2R(10);
      rd[1].Dec = D2R(y * 10);

      rd[2].Ra = rd[1].Ra;
      rd[2].Dec = D2R(y * 10) - D2R(10);

      rd[3].Ra = D2R(x * 10);
      rd[3].Dec = rd[2].Dec;

      for (int i = 0; i < 4; i++)
      {
        rd[i].Dec -= cAstro.getInvAtmRef(rd[i].Dec, 4);
        trfRaDecToPointNoCorrect(&rd[i], &pt[i], &mat);
      }

      if (SKPLANECheckFrustumToPolygon(trfGetFrustum(), pt, 4))
      {
        for (int i = 0; i < 4; i++)
        {
          trfProjectPointNoCheck(&pt[i]);
        }

        scanRender.resetScanPoly(pImg->width(), pImg->height());
        scanRender.scanLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy, ox, oy, ox + tw, oy);
        scanRender.scanLine(pt[1].sx, pt[1].sy, pt[2].sx, pt[2].sy, ox + tw, oy, ox + tw, oy + th);
        scanRender.scanLine(pt[2].sx, pt[2].sy, pt[3].sx, pt[3].sy, ox + tw, oy + th, ox, oy + th);
        scanRender.scanLine(pt[3].sx, pt[3].sy, pt[0].sx, pt[0].sy, ox, oy + th, ox, oy);
        scanRender.renderPolygonAlpha(pImg, bkTexture);

        /*
        p->drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);
        p->drawLine(pt[1].sx, pt[1].sy, pt[2].sx, pt[2].sy);
        p->drawLine(pt[2].sx, pt[2].sy, pt[3].sx, pt[3].sy);
        p->drawLine(pt[3].sx, pt[3].sy, pt[0].sx, pt[0].sy);
        */
      }
    }
    oy += th;
  }
}

/////////////////////////////////////////////////////////////////////
void CBackground::renderHorizonBk(mapView_t *mapView, CSkPainter *p, QImage *pImg)
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
  setSetFont(FONT_HORIZON, p);

  if (isValid)
  {
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
  }

  if (isTexture)
  {
    if (!g_skSet.map.hor.hideTextureWhenMove || !*g_bMouseMoveMap)
    {
      renderTexture(mapView, p, pImg);
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

    p->drawText(rc, Qt::AlignCenter, text);
  };

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

  if (g_skSet.map.hor.showDirections)
  {
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
  }
}

