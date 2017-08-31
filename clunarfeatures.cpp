#include "clunarfeatures.h"
#include "transform.h"
#include "mainwindow.h"
#include "cconsole.h"
#include "setting.h"
#include "mapobj.h"
#include "colongitude.h"

// http://www.fourmilab.ch/earthview/lunarform/lunarform.html

static const double    mkm = 1738 * 2;

CLunarFeatures cLunarFeatures;

QDataStream& operator<<(QDataStream& out, const lfParam_t& v)
{
  out << v.bShowLabels << v.bShowDiam << v.bShowLF << v.filter << v.maxKmDiam << v.minDetail;
  return out;
}

QDataStream& operator>>(QDataStream& in, lfParam_t& v)
{
  in >> v.bShowLabels;
  in >> v.bShowDiam;
  in >> v.bShowLF;
  in >> v.filter;
  in >> v.maxKmDiam;
  in >> v.minDetail;

  return in;
}


////////////////////////////////
CLunarFeatures::CLunarFeatures()
////////////////////////////////
{
}

////////////////////////////////////////////////////////////
static bool sort(const lunarItem_t &a, const lunarItem_t &b)
////////////////////////////////////////////////////////////
{
  // sort by size
  return a.rad < b.rad;
}

///////////////////////////////////////
void CLunarFeatures::load(QString name)
///////////////////////////////////////
{
  SkFile f(name);

  if (!f.open(SkFile::ReadOnly | SkFile::Text))
    return;

  QTextStream s(&f);

  s.readLine(); // first row

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

    list = str.split("\t");

    item.name = list[0].simplified();
    item.lat = D2R(list[3].toDouble());
    item.lon = D2R(list[4].toDouble());
    item.rad = list[2].toDouble();
    item.desc = list[8].simplified();

    if (list[6] == "LS") item.type = LFT_LANDING_SITE;
      else
    if (list[6] == "SF") item.type = LFT_SURF_FEATURE;
      else
    if (list[6] == "RI") item.type = LFT_RIMA;
      else
    if (list[6] == "LC") item.type = LFT_LACUS;
      else
    if (list[6] == "VA") item.type = LFT_VALLIS;
      else
    if (list[6] == "SI") item.type = LFT_SINUS;
      else
    if (list[6] == "AA") item.type = LFT_CRATER;
      else
    if (list[6] == "MO") item.type = LFT_MONS;
      else
    if (list[6] == "OC" || list[6] == "ME") item.type = LFT_MARE;
      else continue;

    tLunarItems.append(item);
  }

  qSort(tLunarItems.begin(), tLunarItems.end(), sort);
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

  SKMATRIXRotateZ(&mY, moon->cMer);
  SKMATRIXRotateY(&mX, moon->cLat);
  SKMATRIXRotateX(&mZ, angle);
  SKMATRIXScale(&mS, 1, view->flipX ? -1 : 1, view->flipY ? -1 : 1);

  mat = mY * mX * mZ * mS;

  p->setFont(setFonts[FONT_LUNAR_FEATURES]);
  p->setBrush(Qt::NoBrush);  

  QFontMetrics fm(p->font());
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

    SKVECTOR in, out;

    cAstro.sphToXYZ(lf->lon, lf->lat, 1, in.x, in.y, in.z);
    SKVECTransform3(&out, &in, &mat);

    if (out.x < 0)
      continue;

    double llon, llat;
    double rr;
    cAstro.xyzToSph(out.x, out.y, out.z, llon, llat, rr);

    int sx = scale *  cos(llat) * sin(llon);
    int sy = scale * -sin(llat);

    sx += pt->sx;
    sy += pt->sy;

    if (!trfPointOnScr(sx, sy, radius))
      continue;

    double d = sqrt(POW2(out.z) + POW2(out.y));

    double r1 = radius;
    double r2 = radius * sqrt(1 - d * d);
    if (r2 > r1)
    {
      r2 = r1;
    }
    double ang = R90 + atan2(-out.z, out.y);

    p->setPen(g_skSet.map.planet.lunarFeatures);

    double opacity = CLAMP((qMax(r1, r2) / 20), 0, 1);

    if (r1 == 0 && r2 == 0)
    {
      p->drawRect(sx - 5, sy - 5, 10, 10);
      p->drawCross(sx, sy, 8);
    }
    else
    {
      if (lf->rad > 200)
        p->setClipPath(pth);

      p->save();
      p->setOpacity(opacity);
      p->translate(sx, sy);
      p->rotate(RAD2DEG(ang));                  
      p->drawEllipse(QPoint(0, 0), (int)r1, (int)r2);      
      p->restore();

      p->setClipping(false);
    }    

    if (par.bShowLabels)
    {
      QString str = lf->name;

      if (par.bShowDiam && lf->type != LFT_LANDING_SITE)
      {
        str += "\n" + QString::number(lf->rad) + (" km");
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

static double distance( double nLat1, double nLon1, double nLat2, double nLon2 )
{
    int nRadius = 1737.1; // Earth's radius in Kilometers
    // Get the difference between our two points
    // then convert the difference into radians
    double nDLat = (nLat2 - nLat1);
    double nDLon = (nLon2 - nLon1);
    double nA = pow ( sin(nDLat/2), 2 ) + cos(nLat1) * cos(nLat2) * pow ( sin(nDLon/2), 2 );

    double nC = 2 * atan2( sqrt(nA), sqrt( 1 - nA ));
    double nD = nRadius * nC;

    return nD; // Return our calculated distance
}


static double distance2(double lat1, double lon1, double lat2, double lon2)
{
  const double moonRadius = 1737.1;
  double u, v;

  u = sin((lat2 - lat1)/2);
  v = sin((lon2 - lon1)/2);

  return 2.0 * moonRadius * asin(sqrt(u * u + cos(lat1) * cos(lat2) * v * v));
}


bool CLunarFeatures::getCoordinates(const mapView_t *view, const QPointF &center, const QPointF &pos, double &lon, double &lat, QString &desc)
{
  orbit_t o;

  cAstro.setParam(view);
  cAstro.calcPlanet(PT_MOON, &o);

  double radius = trfGetArcSecToPix(o.sx);

  double x = (pos.x() - center.x()) / radius;
  double y = (pos.y() - center.y()) / radius;
  double z = 0;

  if (x < -1 || x > 1) return false;
  if (y < -1 || y > 1) return false;

  double p = sqrt(POW2(x) + POW2(y));
  if (p >= 1.) return false;
  double c = asin(p);

  if (p != 0)
  {
    lat = -asin((y  * sin(c)) / p);
    lon =  atan((x * sin(c)) / (p * cos(c)));
  }
  else
  {
    lon = 0;
    lat = 0;
  }

  cAstro.sphToXYZ(lon, lat, 1, x, y, z);

  SKMATRIX     mat;
  SKMATRIX     mX, mY, mZ, mS;

  double angle;

  if (view->flipX + view->flipY == 1)
  {
    angle = o.PA + trfGetAngleToNPole(o.lRD.Ra, o.lRD.Dec, view->jd) + R180;
  }
  else
  {
    angle = o.PA - trfGetAngleToNPole(o.lRD.Ra, o.lRD.Dec, view->jd) + R180;
  }

  if (view->flipY)
  {
    angle = angle + R180;
  }

  SKMATRIXRotateZ(&mY, -o.cMer);
  SKMATRIXRotateY(&mX, -o.cLat);
  SKMATRIXRotateX(&mZ, -angle);
  SKMATRIXScale(&mS, 1, view->flipX ? -1 : 1, view->flipY ? -1 : 1);

  mat = mS * mZ * mX * mY;

  SKVECTOR out;
  SKVECTOR in = SKVECTOR(x, y, z);

  SKVECTransform3(&out, &in, &mat);

  double r;
  cAstro.xyzToSph(out.x, out.y, out.z, lon, lat, r);
  if (lon >= R180) lon = lon - R360;

  desc.clear();

  lunarItem_t item;
  foreach (item, tLunarItems)
  {
    double d = distance(item.lat, item.lon, lat, lon);
    double rad = item.rad * 0.5;
    if (rad == 0) rad = 10;
    if (d <= rad)
    {
      desc = "<b>" + item.name + "</b><br>" + item.desc;
      return true;
    }
  }

  return true;
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
    case LFT_SURF_FEATURE:
      return tr("Lettered crater");
    case LFT_MONS:
      return tr("Mons/Montes");
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


