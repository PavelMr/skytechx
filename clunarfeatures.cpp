#include "clunarfeatures.h"
#include "transform.h"
#include "mainwindow.h"
#include "cconsole.h"
#include "setting.h"
#include "mapobj.h"

// http://www.fourmilab.ch/earthview/lunarform/lunarform.html

static const double    mkm = 1738 * 2;

CLunarFeatures cLunarFeatures;

////////////////////////////////
CLunarFeatures::CLunarFeatures()
////////////////////////////////
{
}

///////////////////////////////////////
void CLunarFeatures::load(QString name)
///////////////////////////////////////
{
  SkFile f(name);

  if (!f.open(SkFile::ReadOnly | SkFile::Text))
    return;

  QTextStream s(&f);

  while (1)
  {
    QString     str = s.readLine();
    QStringList list;

    if (s.atEnd())
      break;

    if (str.isEmpty())
      continue;

    if (str.startsWith("#")) // comment
      continue;

    lunarItem_t item;

    list = str.simplified().split("|");

    if (list.count() != 5)
      continue;

    QString lat = list.at(2).simplified();
    lat.chop(1);

    QString lon = list.at(3).simplified();
    lon.chop(1);

    item.name = list.at(1).trimmed();
    item.rad = list.at(4).toDouble();
    item.lon = D2R(lon.toDouble());
    item.lat = D2R(lat.toDouble());

    if (item.lon > 110)
      continue; // never seen

    if (list.at(2).endsWith('S'))
      item.lat = -item.lat;

    if (list.at(3).endsWith('W'))
      item.lon = -item.lon;

    if (list[0].startsWith("CR"))
      item.type = LFT_CRATER;
    else
    if (list[0].startsWith("LL"))
      item.type = LFT_LANDING_SITE;
    else
    if (list[0].startsWith("MO"))
      item.type = LFT_MONS;
    else
    if (list[0].startsWith("MS"))
      item.type = LFT_MONTES;
    else
    if (list[0].startsWith("RI"))
      item.type = LFT_RIMA;
    else
    if (list[0].startsWith("MR"))
      item.type = LFT_MARE;
    else
    if (list[0].startsWith("VA"))
      item.type = LFT_VALLIS;
    else
    if (list[0].startsWith("LA"))
      item.type = LFT_LACUS;
    else
    if (list[0].startsWith("SI"))
      item.type = LFT_SINUS;
    else
      item.type = LFT_UNKNOWN;

    tLunarItems.append(item);

    //qDebug("%d '%s' %f %f %f", item.type, qPrintable(item.name), R2D(item.lat), R2D(item.lon), item.rad);
    //if (tLunarItems.count() == 15) break;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////
void CLunarFeatures::draw(CSkPainter *p, SKPOINT *pt, int rad, orbit_t *moon, mapView_t *view)
//////////////////////////////////////////////////////////////////////////////////////////////
{
  SKMATRIX  mat;
  SKMATRIX  mX, mY, mZ, mS;
  double    scale = rad;
  lfParam_t par;

  pcMainWnd->lfGetParam(&par);

  if (!par.bShowLF)
    return;

  lunarItem_t item;
  lunarItem_t *lf;

  double angle;

  if (view->flipX + view->flipY == 1)
  {
    angle = moon->PA + trfGetAngleToNPole(moon->lRD.Ra, moon->lRD.Dec, view->jd) + R180;
  }
  else
  {
    angle = moon->PA - trfGetAngleToNPole(moon->lRD.Ra, moon->lRD.Dec, view->jd) + R180;
  }

  if (view->flipY)
  {
    angle = angle + R180;
  }

  SKMATRIXRotateY(&mY, R90 + R180 + moon->cMer);
  SKMATRIXRotateX(&mX, R180 + moon->cLat);
  SKMATRIXRotateZ(&mZ, -angle);
  SKMATRIXScale(&mS, view->flipX ? -1 : 1, view->flipY ? -1 : 1, 1);

  mat = mY * mX * mZ * mS;

  p->setFont(setFonts[FONT_LUNAR_FEATURES]);
  p->setBrush(Qt::NoBrush);
  //p->setBrush(QColor(200, 100, 100, 64));

  QFontMetrics  fm(p->font());

  QPainterPath pth;

  pth.addEllipse(QPoint(pt->sx, pt->sy), rad, rad);

  foreach (item, tLunarItems)
  {
    lf = &item;

    if ((par.filter & (1 << lf->type)) != (1 << lf->type))
      continue;

    double radius = (lf->rad / mkm) * scale;

    if (radius > 0)
    {
      if (lf->rad < par.maxKmDiam)
        continue;

      if (radius < par.minDetail)
        continue;
    }

    SKVECTOR out;
    SKVECTOR in;

    double clat = cos(lf->lat);
    in.x = clat * cos(-lf->lon) * scale;
    in.y =        sin(lf->lat)  * scale;
    in.z = clat * sin(-lf->lon) * scale;

    SKVECTransform3(&out, &in, &mat);

    if (out.z > 0)
      continue;

    int sx = pt->sx + out.x;
    int sy = pt->sy + out.y;

    if (!trfPointOnScr(sx, sy, radius))
      continue;

    double d = sqrt(POW2(out.x) + POW2(out.y)) / scale;

    double r1 = radius;
    double r2 = radius * sqrt(1 - d * d);
    if (r2 > r1)
    {
      r2 = r1;
    }
    double ang = atan2(out.x, -out.y);

    p->setPen(g_skSet.map.planet.lunarFeatures);

    if (r1 == 0 && r2 == 0)
    {
      p->drawRect(sx - 5, sy - 5, 10, 10);
      p->drawCross(sx, sy, 8);
    }
    else
    {
      if (lf->rad > 300)
        p->setClipPath(pth);

      p->save();
      p->translate(sx, sy);
      p->rotate(RAD2DEG(ang));
      p->drawEllipse(QPoint(0, 0), (int)r1, (int)r2);
      p->restore();

      p->setClipping(false);
    }

    if (1)
    {
      QString str = lf->name;

      if (par.bShowDiam && lf->type != LFT_LANDING_SITE)
      {
        str += " " + QString::number(lf->rad) + (" km");
      }

      int tw = fm.width(str);

      setSetFontColor(FONT_LUNAR_FEATURES, p);

      if (tw + 10 < qMin(r1, r2) * 2)
      {
        p->drawCText(sx, sy, str);
      }
      else
      {
        float h = (r1 * r2) / sqrt(POW2(r1) * POW2(cos(ang)) + POW2(r2) * POW2(sin(ang)));

        if (lf->type == LFT_LANDING_SITE)
        {
          h = 0;
        }
        p->drawCText(sx, sy + h + fm.height(), str);
      }
    }
  }
}

static void calcAngularDistance(double ra, double dec, double angle, double distance, double &raOut, double &decOut)
{
  // http://www.movable-type.co.uk/scripts/latlong.html

  decOut = asin(sin(dec) * cos(distance) + cos(dec) * sin(distance) * cos(-angle));
  raOut = ra + atan2(sin(-angle) * sin(distance) * cos(dec), cos(distance) - sin(dec) * sin(decOut));
}

QStringList CLunarFeatures::getNames()
{
  QStringList list;

  foreach (const lunarItem_t &item, tLunarItems)
  {
    list.append(item.name);
  }

  return list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLunarFeatures::search(QString str, mapView_t *view, double &ra, double &dec, double &fov, int searchIndex)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  lunarItem_t item;
  lunarItem_t *lf;
  orbit_t      o;
  SKPOINT      pt;
  SKMATRIX     mat;
  SKMATRIX     mX, mY, mZ, mS;

  cAstro.setParam(view);
  cAstro.calcPlanet(PT_MOON, &o);

  double angle = o.PA;

  SKMATRIXRotateY(&mY, R90 + R180 + o.cMer);
  SKMATRIXRotateX(&mX, R180 + o.cLat);
  SKMATRIXRotateZ(&mZ, -angle);
  SKMATRIXScale(&mS, view->flipX ? -1 : 1, view->flipY ? -1 : 1, 1);

  mat = mY * mX * mZ * mS;

  int i = 0;
  foreach (item, tLunarItems)
  {
    lf = &item;
    if (!str.compare(lf->name, Qt::CaseInsensitive) || searchIndex == i)
    {
      SKVECTOR out;
      SKVECTOR in;

      double clat = cos(lf->lat);
      in.x = clat * cos(-lf->lon);
      in.y =        sin(lf->lat);
      in.z = clat * sin(-lf->lon);

      SKVECTransform3(&out, &in, &mat);

      if (out.z > 0) // on far side of the moon
        return(false);

      double rad = D2R(o.sx / 3600.0 / 2.);
      double vec[2];

      vec[0] = (view->flipX ? -1 : 1) * out.x;
      vec[1] = (view->flipY ? -1 : 1) * out.y;

      double ang = atan2(vec[0], -vec[1]);
      calcAngularDistance(o.lRD.Ra, o.lRD.Dec, ang, sqrt(POW2(out.x) + POW2(out.y)) * rad, ra, dec);
      fov = D2R(0.001 * lf->rad);
      if (fov < D2R(0.2))
      {
        fov = D2R(0.2);
      }
      else
      if (fov > D2R(1))
      {
        fov = D2R(1);
      }

      return(true);
    }
    i++;
  }

  return(false);
}

bool CLunarFeatures::isVisible(int index, mapView_t *view)
{
  lunarItem_t *lf = &tLunarItems[index];
  orbit_t      o;
  SKPOINT      pt;
  SKMATRIX     mat;
  SKMATRIX     mX, mY, mZ, mS;

  cAstro.setParam(view);
  cAstro.calcPlanet(PT_MOON, &o);

  double angle = o.PA;

  SKMATRIXRotateY(&mY, R90 + R180 + o.cMer);
  SKMATRIXRotateX(&mX, R180 + o.cLat);
  SKMATRIXRotateZ(&mZ, -angle);
  SKMATRIXScale(&mS, view->flipX ? -1 : 1, view->flipY ? -1 : 1, 1);

  mat = mY * mX * mZ * mS;

  SKVECTOR out;
  SKVECTOR in;

  double clat = cos(lf->lat);
  in.x = clat * cos(-lf->lon);
  in.y =        sin(lf->lat);
  in.z = clat * sin(-lf->lon);

  SKVECTransform3(&out, &in, &mat);

  return out.z < 0;
}

QString CLunarFeatures::getTypeName(int id)
{
  switch (id)
  {
    case LFT_LANDING_SITE:
      return tr("Landing site");
    case LFT_CRATER:
      return tr("Crater");
    case LFT_MONTES:
      return tr("Montes");
    case LFT_MONS:
      return tr("Mons");
    case LFT_RIMA:
      return tr("Rima");
    case LFT_MARE:
      return tr("Mare");
    case LFT_VALLIS:
      return tr("Vallis");
    case LFT_LACUS:
      return tr("Lacus");
    case LFT_SINUS:
      return tr("Sinus");
  }

  return "???";
}


