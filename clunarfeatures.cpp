#include "clunarfeatures.h"
#include "transform.h"
#include "mainwindow.h"
#include "cconsole.h"
#include "setting.h"

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
      continue; // never be seen

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


void getRaDecOffset(double ra, double dec, double offx, double offy, double &outRa, double &outDec)
{
  // FIXME: nefunguje to spravne
  SKMATRIX  mat1;
  SKMATRIX  mat2;
  SKMATRIX  mat;

  SKMATRIX  mX, mY;
  SKMATRIX  mX2, mY2;

  SKMATRIXRotateY(&mY, offy);
  SKMATRIXRotateZ(&mX, offx);

  mat1 = mY * mX;

  SKMATRIXRotateY(&mY2, -ra);
  SKMATRIXRotateZ(&mX2, dec);

  mat2 = mY2 * mX2;

  mat = mat1 * mat2;

  SKVECTOR out;
  SKVECTOR in;

  double clat = cos(0.);

  in.x = clat * cos(0.);
  in.y =        sin(0.);
  in.z = clat * sin(0.);


  SKVECTransform3(&out, &in, &mat);

  outRa  = atan2(out.y, out.x);
  outDec = atan2(out.z, sqrt(POW2(out.x) + POW2(out.y)));
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

  double angle = moon->PA - trfGetAngleToNPole(moon->lRD.Ra, moon->lRD.Dec) + R180;
  SKMATRIXRotateY(&mY, R90 + R180 + moon->cMer);
  SKMATRIXRotateX(&mX, R180 + moon->cLat);
  SKMATRIXRotateZ(&mZ, -angle);
  SKMATRIXScale(&mS, view->flipX ? -1 : 1, view->flipY ? -1 : 1, 1);

  mat = mY * mX * mZ * mS;

  p->setFont(setFonts[FONT_LUNAR_FEATURES]);
  p->setBrush(Qt::NoBrush);

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

    double d = sqrt(POW2(out.x / scale) + POW2(out.y / scale)) * R90;

    double r1 = radius;
    double r2 = radius * (cos(d) * 2);
    if (r2 > r1)
      r2 = r1;
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

    // TODO: lepsi align
    QString str = lf->name;

    if (par.bShowDiam && lf->type != LFT_LANDING_SITE)
    {
      str += " " + QString::number(lf->rad) + (" km");
    }

    int tw = fm.width(str);

    setSetFontColor(FONT_LUNAR_FEATURES, p);

    if (tw + 10 < qMin(r1, r2) * 2)
      p->drawCText(sx, sy, str);
    else
      p->drawCText(sx, sy + qMax(r1, r2) + fm.height() - 5, str);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool CLunarFeatures::search(QString str, mapView_t *view, double &ra, double &dec, double &fov)
//////////////////////////////////////////////////////////////////////////////////////////////
{
  lunarItem_t item;
  lunarItem_t *lf;
  orbit_t      o;
  SKPOINT      pt;
  SKMATRIX     mat;
  SKMATRIX     mX, mY, mZ, mS;

  return false;

  cAstro.calcPlanet(PT_MOON, &o);

  double angle = o.PA - trfGetAngleToNPole(o.lRD.Ra, o.lRD.Dec) + R180;
  SKMATRIXRotateY(&mY, R90 + R180 + o.cMer);
  SKMATRIXRotateX(&mX, R180 + o.cLat);
  SKMATRIXRotateZ(&mZ, -angle);
  SKMATRIXScale(&mS, view->flipX ? -1 : 1, view->flipY ? -1 : 1, 1);

  mat = mY * mX * mZ * mS;

  foreach (item, tLunarItems)
  {
    lf = &item;

    if (!str.compare(lf->name, Qt::CaseInsensitive))
    {
      //double radius = (lf->rad / mkm);// * scale;

      SKVECTOR out;
      SKVECTOR in;

      double clat = cos(lf->lat);
      in.x = clat * cos(-lf->lon);// * scale;
      in.y =        sin(lf->lat);// * scale;
      in.z = clat * sin(-lf->lon);// * scale;

      SKVECTransform3(&out, &in, &mat);

      if (out.z > 0) // on far side of the moon
        return(false);

      double rad = D2R(o.sx / 3600.0 / 2.);

      //getRaDecOffset(o.lRD.Ra, o.lRD.Dec, out.x * rad, out.y * rad, ra, dec);

      ra =  o.lRD.Ra -  out.x * rad;
      dec = o.lRD.Dec - out.y * rad;

      fov = D2R(0.006 * lf->rad);
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
  }

  return(false);
}


