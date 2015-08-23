#include "cobjfillinfo.h"
#include "smap.h"
#include "crts.h"
#include "casterdlg.h"
#include "ccomdlg.h"
#include "csatxyz.h"
#include "dsoplug.h"
#include "Usno2A.h"
#include "cucac4.h"
#include "csgp4.h"

#define FILEREGEXP   QRegExp("\\W")

static QString gscMB[19][3] = {{"IIIaJ","GG395","SERC-J/EJ"},    //0
                               {"IIaD","W12","Pal Quick-V"},     //1
                               {"","",""},                       //2
                               {"","B","Johnson"},               //3
                               {"","V","Johnson"},               //4
                               {"IIIaf","RG630","Red"},          //5
                               {"IIaD","GG495","Pal QV/AAO XV"}, //6
                               {"103aO","","POSS-I Blue"},       //7
                               {"103aE","Red Plex","POSS-I Red"},//8
                               {"","",""},                       //9
                               {"IIaD","GG495","GPO Astrograph"},//10
                               {"103aO","GG400","Black Birch Astrograph"},//11
                               {"","",""},
                               {"","",""},
                               {"","",""},
                               {"","",""},
                               {"IIIaJ","GG495","QV"},//16
                               {"","",""},
                               {"IIIaJ","GG385","POSS-II Blue"}};



////////////////////////////
CObjFillInfo::CObjFillInfo()
////////////////////////////
{
  txDateTime = tr("Date & Time");
  txRA       = tr("Right Ascension");
  txObjType  = tr("Object type");
  txDesig    = tr("Designation");
  txDec      = tr("Declination");
  txLocInfo  = tr("Local information");
  txVisMag   = tr("Visual magnitude");
  txConstel  = tr("Constellation");
  txElongation = tr("Elongation");
  txJ2000      = tr(" (J2000.0)");

  gscClass[0] = tr("Star");
  gscClass[1] = tr("Galaxy");
  gscClass[2] = tr("Unknown object");
  gscClass[3] = tr("Deep sky object / Plate defect");
  gscClass[4] = tr("Unspecified");
  gscClass[5] = tr("Artefact");
}

////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
////////////////////////////////////////////////////////////////////////////////////////
{
  item->mapObj = *obj;

  item->type = obj->type;
  item->par1 = obj->par1;
  item->par2 = obj->par2;
  item->jd = view->jd;

  switch (obj->type)
  {
    case MO_PLN_SAT:
      fillPlnSatInfo(view, obj, item);
      break;

    case MO_TYCSTAR:
      fillTYCInfo(view, obj, item);
      break;

    case MO_UCAC4:
      fillUCAC4Info(view, obj, item);
      break;

    case MO_GSCSTAR:
      fillGSCInfo(view, obj, item);
      break;

    case MO_PPMXLSTAR:
      fillPPMXLInfo(view, obj, item);
      break;

    case MO_USNOSTAR:
      fillUSNOInfo(view, obj, item);
      break;

    case MO_DSO:
      fillDSOInfo(view, obj, item);
      break;

    case MO_PLANET:
      fillPlanetInfo(view, obj, item);
      break;

    case MO_ASTER:
      fillAsterInfo(view, obj, item);
      break;

    case MO_COMET:
      fillCometInfo(view, obj, item);
      break;

    case MO_EARTH_SHD:
      fillESInfo(view, obj, item);
      break;

    case MO_SATELLITE:
      fillSatelliteInfo(view, obj, item);
      break;
  }
}


////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillAtlas(double ra, double dec, ofiItem_t *item)
////////////////////////////////////////////////////////////////////
{
  QString tmpStr;

  addLabelItem(item, tr("Atlases"));
  addSeparator(item);

  getHO2000(tmpStr, ra, dec);
  addTextItem(item, tr("Hvezdna obloha 2000.0"), tmpStr);
  getMilleniumStasAtlas(tmpStr, ra, dec);
  addTextItem(item, tr("Millennium Star Atlas"), tmpStr);
  getStarAtlas2000(tmpStr, ra, dec);
  addTextItem(item, tr("Sky Atlas 2000.0"), tmpStr);
  getUranometriaAtlas2000(tmpStr, ra, dec);
  addTextItem(item, tr("Uranometria 2000"), tmpStr);
  getInterstellarumDeepSkyAtlas(tmpStr, ra, dec);
  addTextItem(item, tr("Interstellarum Deep Sky Atlas"), tmpStr);

  addSeparator(item);
}


//////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillRTS(rts_t *rts, const mapView_t *view, ofiItem_t *item)
//////////////////////////////////////////////////////////////////////////////
{
  double tz = view->geo.tz;

  switch (rts->flag)
  {
    case RTS_ERR:
      addTextItem(item, tr("Rise/Set solve ERROR!!!"), tr("N/A"), true);
      item->rtsType = RTS_ERR;
      break;

    case RTS_CIRC:
      {
        addTextItem(item, tr("Object is circumpolar."), "");
        addTextItem(item, tr("Transit"), getStrTime(rts->transit, tz) + " " + tr("Alt : ") + getStrDegDF(rts->tAlt));
        item->rtsType = RTS_CIRC;
        item->riseJD = CM_UNDEF;
        item->setJD = CM_UNDEF;
        item->transitJD = rts->transit;
      }
      break;

    case RTS_NONV:
      addTextItem(item, tr("Object is never visible!"), "");
      addTextItem(item, tr("Transit"), getStrTime(rts->transit, tz) + " " + tr("Alt : ") + getStrDegDF(rts->tAlt));
      item->rtsType = RTS_NONV;
      item->riseJD = CM_UNDEF;
      item->setJD = CM_UNDEF;
      item->transitJD = CM_UNDEF;
      break;

    case RTS_DONE:
      item->rtsType = RTS_DONE;
      item->riseJD = CM_UNDEF;
      item->setJD = CM_UNDEF;
      item->transitJD = CM_UNDEF;

      if ((rts->rts & RTS_T_RISE) == RTS_T_RISE)
      {
        addTextItem(item, tr("Rise"), getStrTime(rts->rise, tz) + " " + tr("Azm : ") + getStrDegDF(rts->rAzm));
        item->riseJD = rts->rise;
      }
      if ((rts->rts & RTS_T_TRANSIT) == RTS_T_TRANSIT)
      {
        addTextItem(item, tr("Transit"), getStrTime(rts->transit, tz) + " " + tr("Alt : ") + getStrDegDF(rts->tAlt));
        item->transitJD = rts->transit;
      }
      if ((rts->rts & RTS_T_SET) == RTS_T_SET)
      {
        addTextItem(item, tr("Set"), getStrTime(rts->set, tz) + " " + tr("Azm : ") + getStrDegDF(rts->sAzm));
        item->setJD = rts->set;
      }

      break;
  }
  addSeparator(item);
}


//////////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillPlnSatInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
//////////////////////////////////////////////////////////////////////////////////////////////
{
  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, txDateTime, QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  satxyz_t sat;
  orbit_t  pl;
  orbit_t  s;

  item->riseJD = CM_UNDEF;
  item->setJD = CM_UNDEF;
  item->transitJD = CM_UNDEF;
  item->rtsType = RTS_ERR;

  cAstro.calcPlanet(obj->par1, &pl);
  cAstro.calcPlanet(PT_SUN, &s);
  cSatXYZ.solve(view->jd, obj->par1, &pl, &s, &sat);

  item->radec.Ra = sat.sat[obj->par2].rd.Ra;
  item->radec.Dec = sat.sat[obj->par2].rd.Dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(0.05));

  precess(&item->radec.Ra, &item->radec.Dec, view->jd, JD2000);

  item->title = sat.sat[obj->par2].name;
  item->simbad = item->title;

  QString str = item->title;

  str = str.remove(FILEREGEXP);
  item->id = "pln_sat_" + str;

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Satellite"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);
  addTextItem(item, sat.sat[obj->par2].name, "");
  addSeparator(item);

  int con = constWhatConstel(sat.sat[obj->par2].rd.Ra,
                             sat.sat[obj->par2].rd.Dec, view->jd);

  double ra = sat.sat[obj->par2].rd.Ra;
  double dec = sat.sat[obj->par2].rd.Dec;
  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txConstel, constGetName(con, 1));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(sat.sat[obj->par2].mag));
  addTextItem(item, tr("Distance from planet center"), getStrDeg(sat.sat[obj->par2].distance));

  /*
  addTextItem(item, tr("In sun light"), QString::number(sat.sat[obj->par2].inSunLgt));
  addTextItem(item, tr("Is hidden"), QString::number(sat.sat[obj->par2].isHidden));
  addTextItem(item, tr("Is transiting"), QString::number(sat.sat[obj->par2].isTransit));
  addTextItem(item, tr("Throw shadow"), QString::number(sat.sat[obj->par2].throwShadow));
  */

  //str = sat.sat[o.par2].name + " " + getStrMag(sat.sat[obj->par2].mag);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillAsterInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
/////////////////////////////////////////////////////////////////////////////////////////////
{
  asteroid_t *a = (asteroid_t *)obj->par2;

  double ra  = a->orbit.lRD.Ra;
  double dec = a->orbit.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  item->radec.Ra = ra;
  item->radec.Dec = dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(2.5));

  item->title = a->name;
  item->simbad = item->title;

  QString str = a->name;

  str = str.remove(FILEREGEXP);
  item->id = "ast_" + str;

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, txDateTime, QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Asteroid"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);
  addTextItem(item, a->name, "");
  addSeparator(item);

  ra = a->orbit.lRD.Ra;
  dec = a->orbit.lRD.Dec;

  int con = constWhatConstel(ra, dec, view->jd);
  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(a->orbit.mag));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, txElongation, QString("%1").arg(R2D(a->orbit.elongation), 0, 'f', 2));

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcOrbitRTS(&rts, obj->par2, MO_ASTER, view);
  fillRTS(&rts, view, item);

  ra  = a->orbit.gRD.Ra;
  dec = a->orbit.gRD.Dec;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, tr("Geocentric information"));
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addTextItem(item, tr("R"), QString::number(a->orbit.R) + tr("AU"));
  addTextItem(item, tr("Light time"), QString::number(a->orbit.light * 24.) + tr(" hours"));
  addSeparator(item);

  addLabelItem(item, tr("Heliocentric information"));
  addSeparator(item);
  addTextItem(item, tr("X"), QString::number(a->orbit.hRect[0]));
  addTextItem(item, tr("Y"), QString::number(a->orbit.hRect[1]));
  addTextItem(item, tr("Z"), QString::number(a->orbit.hRect[2]));
  addSeparator(item);
  addTextItem(item, tr("Longitude"), QString::number(R2D(a->orbit.hLon), 'f', 8));
  addTextItem(item, tr("Latitude"), QString::number(R2D(a->orbit.hLat), 'f', 8));
  addSeparator(item);
  addTextItem(item, tr("r"), QString::number(a->orbit.r) + tr("AU"));
  addSeparator(item);

  ra  = a->orbit.lRD.Ra;
  dec = a->orbit.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(ra));
  addTextItem(item, txDec, getStrDeg(dec));
  addSeparator(item);

  fillAtlas(ra, dec, item);
}


/////////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillCometInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
/////////////////////////////////////////////////////////////////////////////////////////////
{
  comet_t *a = (comet_t *)obj->par2;

  double ra  = a->orbit.lRD.Ra;
  double dec = a->orbit.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  item->radec.Ra = ra;
  item->radec.Dec = dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(2.5));

  item->title = a->name;
  item->simbad = item->title;

  QString str = a->name;

  str = str.remove(FILEREGEXP);
  item->id = "com_" + str;

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Comet"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);
  addTextItem(item, a->name, "");
  addSeparator(item);

  ra = a->orbit.lRD.Ra;
  dec = a->orbit.lRD.Dec;

  int con = constWhatConstel(ra, dec, view->jd);

  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(a->orbit.mag));
  addTextItem(item, txConstel, constGetName(con, 1));

  addSeparator(item);
  addTextItem(item, txElongation, QString("%1").arg(R2D(a->orbit.elongation), 0, 'f', 2));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcOrbitRTS(&rts, obj->par2, MO_COMET, view);
  fillRTS(&rts, view, item);

  ra  = a->orbit.gRD.Ra;
  dec = a->orbit.gRD.Dec;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, tr("Geocentric information"));
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addTextItem(item, tr("R"), QString::number(a->orbit.R) + tr("AU"));
  addTextItem(item, tr("Light time"), QString::number(a->orbit.light * 24.) + tr(" hours"));
  addSeparator(item);

  addLabelItem(item, tr("Heliocentric information")  + " " + tr("J2000.0"));
  addSeparator(item);
  addTextItem(item, tr("X"), QString::number(a->orbit.hRect[0]));
  addTextItem(item, tr("Y"), QString::number(a->orbit.hRect[1]));
  addTextItem(item, tr("Z"), QString::number(a->orbit.hRect[2]));
  addSeparator(item);
  addTextItem(item, tr("Longitude"), QString::number(R2D(a->orbit.hLon), 'f', 8));
  addTextItem(item, tr("Latitude"), QString::number(R2D(a->orbit.hLat), 'f', 8));
  addSeparator(item);
  addTextItem(item, tr("r"), QString::number(a->orbit.r) + tr("AU"));
  addSeparator(item);

  addLabelItem(item, tr("Other"));
  addSeparator(item);

  QString label = tr("Orbital period");

  if (a->e < 0.99)
  {
    double aa = a->q / (1.0 - a->e);
    double P = 1.00004024 * pow(aa, 1.5);
    addTextItem(item, label, QString("%1").arg(P) + tr(" years"));
  }
  else
  {
    addTextItem(item, label, tr("Non periodic"));
  }

  double v;
  if (a->e < 1)
  {
    double ap = a->q / (1 - a->e);
    v = 42.1219 * sqrt((1 / a->orbit.r) - (1 / (2 * ap)));
  }
  else
    v = 42.1219 * sqrt(1 / a->orbit.r);

  addTextItem(item, tr("Current speed"), QString("%1").arg(v, 0, 'f', 3) + tr(" km/s"));
  addSeparator(item);

  ra  = a->orbit.lRD.Ra;
  dec = a->orbit.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(ra));
  addTextItem(item, txDec, getStrDeg(dec));
  addSeparator(item);

  fillAtlas(ra, dec, item);
}

void CObjFillInfo::fillSatelliteInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
{
  QString str;
  satellite_t s;
  radec_t rd;
  double ra2000, dec2000;

  sgp4.solve(obj->par1, view, &s);
  tleItem_t *tle = sgp4.tleItem(obj->par1);

  cAstro.convAA2RDRef(s.azimuth, s.elevation, &rd.Ra, &rd.Dec);

  ra2000 = rd.Ra;
  dec2000 = rd.Dec;
  precess(&ra2000, &dec2000, view->jd, JD2000);

  item->radec.Ra = ra2000;
  item->radec.Dec = dec2000;
  item->zoomFov = getOptObjFov(0, 0, D2R(2.5));

  str = s.name;
  str = str.remove(FILEREGEXP);
  item->id = "esat_" + str;

  item->title = s.name;
  item->simbad = item->id;

  item->riseJD = CM_UNDEF;
  item->setJD = CM_UNDEF;
  item->transitJD = CM_UNDEF;
  item->rtsType = RTS_ERR;

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Satellite"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  addTextItem(item, s.name, "");
  addSeparator(item);

  int con = constWhatConstel(rd.Ra, rd.Dec, view->jd);
  QString jd2000;
  double ra = rd.Ra;
  double dec = rd.Dec;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txConstel, constGetName(con, 1));

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(s.azimuth));
  addTextItem(item, tr("Altitude"), getStrDeg(s.elevation));
  addTextItem(item, tr("Range"), QString("%1").arg(s.range, 0, 'f', 1) + tr(" km."));
  addSeparator(item);

  addLabelItem(item, tr("Geocentric information"));
  addSeparator(item);
  addTextItem(item, tr("Longitude"), getStrDeg(s.longitude));
  addTextItem(item, tr("Latitude"), getStrDeg(s.latitude));
  addTextItem(item, tr("Altitude"), QString("%1").arg(s.altitude, 0, 'f', 1) + tr(" km."));
  addSeparator(item);

  addLabelItem(item, tr("Other"));
  addSeparator(item);
  addTextItem(item, tr("Orbital period"), QString("%1").arg(tle->period, 0, 'f', 1) + tr(" min."));
  addTextItem(item, tr("Inclination"), getStrDeg(tle->inclination));
  addTextItem(item, tr("Perigee"), QString("%1").arg(tle->perigee, 0, 'f', 1) + tr(" km."));
  addTextItem(item, tr("Epoch"), QString("%1 / %2").arg(getStrDate(tle->epoch, view->geo.tz)).arg(getStrTime(tle->epoch, view->geo.tz)));

  double diff = view->jd - tle->epoch;
  addTextItem(item, tr("Time difference"), QString("%1").arg(diff, 0, 'f', 1) + tr(" day(s)"));

  addSeparator(item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "SGP4 C++ Satellite Library", "");
}

///////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillTYCInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
///////////////////////////////////////////////////////////////////////////////////////////
{
  tychoStar_t *t;
  QString      str;

  cTYC.getStar(&t, obj->par1, obj->par2);

  item->radec.Ra = t->rd.Ra;
  item->radec.Dec = t->rd.Dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(2.5));
  item->id = QString("TYC%1-%2-%3").arg(t->tyc1).arg(t->tyc2).arg(t->tyc3);
  item->simbad = item->id;

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Star (TYC2 cat.)"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  int con = constWhatConstel(t->rd.Ra, t->rd.Dec, JD2000);
  item->title = QString("TYC %1-%2-%3").arg(t->tyc1).arg(t->tyc2).arg(t->tyc3);

  if (t->supIndex != -1)
  {
    bool         found;
    tychoSupp_t *supp = &cTYC.pSupplement[t->supIndex];

    str = cTYC.getStarName(supp);
    if (str.length() > 0)
    {
      addTextItem(item, str, "");
      item->title = str;
    }
    str = cTYC.getBayerFullStr(supp, found);
    if (found)
      addTextItem(item, str + " " + constGetName(con, 2), "");
    str = cTYC.getFlamsteedStr(supp, found);
    if (found)
      addTextItem(item, str + " " + constGetName(con, 2), "");
    addTextItem(item, "HD " + QString::number(supp->hd), "");
  }

  str = QString("TYC %1-%2-%3").arg(t->tyc1).arg(t->tyc2).arg(t->tyc3);
  addTextItem(item, str, "");
  addSeparator(item);

  double ra, dec;
  QString jd2000;

  ra = t->rd.Ra;
  dec = t->rd.Dec;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
  }
  else
  {
    precess(&ra, &dec, JD2000, view->jd);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(cTYC.getVisMag(t)));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcFixed(&rts, ra, dec, view);
  fillRTS(&rts, view, item);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(t->rd.Ra));
  addTextItem(item, txDec, getStrDeg(t->rd.Dec));
  addSeparator(item);

  addLabelItem(item, tr("Apparent view"));
  addSeparator(item);

  if (t->supIndex != -1)
  {
    tychoSupp_t *supp = &cTYC.pSupplement[t->supIndex];

    if (supp->spt != 0)
    {
      QString spa[] = {"N/A", "O", "B", "A", "F", "G", "K", "M"};

      addTextItem(item, tr("Spectral type"), spa[supp->spt] + QString(supp->spn));
    }
  }
  double mag = 0.850 * (TYC_SHORT_TO_MAG(t->BTmag) - TYC_SHORT_TO_MAG(t->VTmag));
  addTextItem(item, tr("B-V mag."), getStrMag(mag));

  addTextItem(item, tr("Johnson BT"), getStrMag(TYC_SHORT_TO_MAG(t->BTmag)));
  addTextItem(item, tr("Johnson VT"), getStrMag(TYC_SHORT_TO_MAG(t->VTmag)));
  addSeparator(item);

  fillAtlas(t->rd.Ra, t->rd.Dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "The Tycho-2 Catalogue (Hog+ 2000)", "");
}


void CObjFillInfo::fillUCAC4Info(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
{
  ucac4Star_t s;

  cUcac4.getStar(s, obj->par1, obj->par2);

  item->radec.Ra = s.rd.Ra;
  item->radec.Dec = s.rd.Dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(0.15));
  item->title = QString("UCAC4 %1-%2").arg(s.zone).arg(s.number, 6, 10, QChar('0'));
  item->simbad = item->title;
  item->id = QString("UCAC4_%1-%2").arg(s.zone).arg(s.number, 6, 10, QChar('0'));

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Star (UCAC4 cat.)"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  int con = constWhatConstel(s.rd.Ra, s.rd.Dec, JD2000);

  addTextItem(item, item->title, "");
  addSeparator(item);

  double ra, dec;

  ra = item->radec.Ra;
  dec = item->radec.Dec;

  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;

  }
  else
  {
    precess(&ra, &dec, JD2000, view->jd);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(s.mag));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcFixed(&rts, ra, dec, view);
  fillRTS(&rts, view, item);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(item->radec.Ra));
  addTextItem(item, txDec, getStrDeg(item->radec.Dec));
  addSeparator(item);

  fillAtlas(item->radec.Ra, item->radec.Dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "The UCAC4 Catalogue (Zacharias+ 2012)", "");
}


///////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillGSCInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
///////////////////////////////////////////////////////////////////////////////////////////
{
  gsc_t        t;
  QString      str;

  cGSC.getStar(&t, obj->par1, obj->par2);

  item->radec.Ra = t.Ra;
  item->radec.Dec = t.Dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(0.25));
  item->title = QString("GSC %1-%2").arg(t.reg).arg(t.id);
  item->simbad = item->title;
  item->id = QString("GSC%1-%2").arg(t.reg).arg(t.id);


  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Star (GSC1.2 cat.)"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  int con = constWhatConstel(t.Ra, t.Dec, JD2000);

  str = QString("GSC %1-%2").arg(t.reg).arg(t.id);
  addTextItem(item, str, "");
  addSeparator(item);

  double ra, dec;

  ra = t.Ra;
  dec = t.Dec;

  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;

  }
  else
  {
    precess(&ra, &dec, JD2000, view->jd);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(t.pMag));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcFixed(&rts, ra, dec, view);
  fillRTS(&rts, view, item);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(t.Ra));
  addTextItem(item, txDec, getStrDeg(t.Dec));
  addSeparator(item);

  addLabelItem(item, tr("Other"));
  addSeparator(item);
  addTextItem(item, tr("Emulsion & Filter"), gscMB[(int)t.magBand][0] + " " +
                                             gscMB[(int)t.magBand][1] + " " +
                                             gscMB[(int)t.magBand][2]);
  addTextItem(item, tr("Identification"), gscClass[(int)t.oClass]);
  addSeparator(item);

  fillAtlas(t.Ra, t.Dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "The HST Guide Star Catalog, Version 1.2 (Lasker+ 1996)", "");
}


/////////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillPPMXLInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
/////////////////////////////////////////////////////////////////////////////////////////////
{
  QString      str;
  ppmxlCache_t *data;

  data = cPPMXL.getRegion(obj->par1);
  ppmxl_t *t = &data->data[obj->par2];

  item->radec.Ra = t->ra / 500000000.0;
  item->radec.Dec = (t->dec / 500000000.0) - R90;
  item->zoomFov = getOptObjFov(0, 0, D2R(0.15));
  item->title = QString("PPMXL %1").arg(t->id);
  item->simbad = item->title;
  item->id = QString("PPMXL%1").arg(t->id);

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Star (PPMXL cat.)"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  int con = constWhatConstel(item->radec.Ra, item->radec.Dec, JD2000);

  str = QString("PPMXL %1").arg(t->id);
  addTextItem(item, str, "");
  addSeparator(item);

  double ra, dec;

  ra = item->radec.Ra;
  dec = item->radec.Dec;
  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;

  }
  else
  {
    precess(&ra, &dec, JD2000, view->jd);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(t->mag / 1000.0));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcFixed(&rts, ra, dec, view);
  fillRTS(&rts, view, item);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(item->radec.Ra));
  addTextItem(item, txDec, getStrDeg(item->radec.Dec));
  addSeparator(item);

  fillAtlas(item->radec.Ra, item->radec.Dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "The PPMXL Catalog (Roeser+ 2010)", "");
}

////////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillUSNOInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
////////////////////////////////////////////////////////////////////////////////////////////
{
  QString     str;
  usnoStar_t  s;
  usnoZone_t *z;

  z = usno.getStar(&s, obj->par1, obj->par2);

  item->radec.Ra = s.rd.Ra;
  item->radec.Dec = s.rd.Dec;
  item->zoomFov = getOptObjFov(0, 0, D2R(0.15));
  item->title = QString("USNO2 %1-%2").arg(z->zone).arg(s.id);
  item->simbad = item->title;
  item->id = QString("USNO2 %1-%2").arg(z->zone).arg(s.id);

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, tr("Star (USNO2 cat.)"));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  int con = constWhatConstel(item->radec.Ra, item->radec.Dec, JD2000);

  str = QString("USNO2 %1-%2").arg(z->zone).arg(s.id);
  addTextItem(item, str, "");
  addSeparator(item);

  double ra, dec;

  ra = item->radec.Ra;
  dec = item->radec.Dec;

  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
  }
  else
  {
    precess(&ra, &dec, JD2000, view->jd);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(s.rMag));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcFixed(&rts, ra, dec, view);
  fillRTS(&rts, view, item);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(item->radec.Ra));
  addTextItem(item, txDec, getStrDeg(item->radec.Dec));
  addSeparator(item);

  fillAtlas(item->radec.Ra, item->radec.Dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "The USNO-A2.0 Catalogue (Monet+ 1998)", "");
}


///////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillDSOInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
///////////////////////////////////////////////////////////////////////////////////////////
{
  dso_t *dso = (dso_t *)obj->par1;
  QString str;

  item->radec.Ra = dso->rd.Ra;
  item->radec.Dec = dso->rd.Dec;
  item->zoomFov = getOptObjFov(dso->sx / 3600.0, dso->sy / 3600.0, D2R(1));
  item->title = cDSO.getCommonName(dso);
  if (item->title.length() == 0)
    item->title = cDSO.getName(dso);

  item->simbad = cDSO.getName(dso);
  item->id = QString("DSO%1").arg(item->title.simplified()); //arg(obj->par1);

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  bool ok;

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, cDSO.getTypeName(dso->type, ok));
  addSeparator(item);

  addLabelItem(item, txDesig);
  addSeparator(item);

  str = cDSO.getCommonName(dso);
  if (str.length() > 0)
    addTextItem(item, str, "");

  for (int i = 0;; i++)
  {
    str = cDSO.getName(dso, i);
    if (str.length() > 0)
      addTextItem(item, str, "");
    else
      break;
  }

  double ra, dec;

  ra = dso->rd.Ra;
  dec = dso->rd.Dec;

  addSeparator(item);

  int con = constWhatConstel(item->radec.Ra, item->radec.Dec, JD2000);

  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;

  }
  else
  {
    precess(&ra, &dec, JD2000, view->jd);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, dso->mag == NO_DSO_MAG ? tr("N/A") : getStrMag(dso->DSO_MAG));
  addTextItem(item, txConstel, constGetName(con, 1));

  double azm, alt;
  double nazm, nalt;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcFixed(&rts, ra, dec, view);
  fillRTS(&rts, view, item);

  addLabelItem(item, tr("Other"));
  addSeparator(item);
  addTextItem(item, "Size", getStrSize(dso->sx, dso->sy));
  if (dso->pa != NO_DSO_PA)
    addTextItem(item, tr("P.A."), QString::number(dso->pa) + "°");
  if (dso->type == DSOT_GALAXY)
    addTextItem(item, tr("Class"), cDSO.getClass(dso));

  addSeparator(item);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(item->radec.Ra));
  addTextItem(item, txDec, getStrDeg(item->radec.Dec));
  addSeparator(item);

  fillAtlas(item->radec.Ra, item->radec.Dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, cDSO.getCatalogue(dso), "");

  QList <CDSOPluginInterface::dsoPlgOut_t> lst = dsoGetPluginDesc(cDSO.getName(dso, 0), cDSO.getName(dso, 1));

  if (lst.count() > 0)
    addSeparator(item);

  for (int i = 0; i < lst.count(); i++)
  {
    addLabelItem(item, lst[i].title);
    addSeparator(item);
    for (int j = 0; j < lst[i].items.count(); j++)
    {
      if (!lst[i].items[j].value.simplified().isEmpty())
      {
        addTextItem(item, lst[i].items[j].label, lst[i].items[j].value);
      }
    }
    addSeparator(item);
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillPlanetInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
//////////////////////////////////////////////////////////////////////////////////////////////
{
  orbit_t o;

  cAstro.calcPlanet(obj->par1, &o);

  double ra  = o.lRD.Ra;
  double dec = o.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  item->radec.Ra = ra;
  item->radec.Dec = dec;
  item->zoomFov = getOptObjFov(o.sx / 3600., o.sy / 3600.);
  item->title = o.name;
  item->simbad = "";//
  item->id = QString("PLN_%1").arg(obj->par1);

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, item->title);
  addSeparator(item);

  ra  = o.lRD.Ra;
  dec = o.lRD.Dec;
  int con = constWhatConstel(ra, dec, view->jd);

  QString jd2000;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, txLocInfo);
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  addSeparator(item);
  addTextItem(item, txVisMag, getStrMag(o.mag));
  addTextItem(item, txConstel, constGetName(con, 1));

  addSeparator(item);
  addTextItem(item, txElongation, QString("%1").arg(R2D(o.elongation), 0, 'f', 2));

  double azm, alt;
  double nazm, nalt;
  QString diam;

  cAstro.convRD2AARef(ra, dec, &azm, &alt);
  cAstro.convRD2AANoRef(ra, dec, &nazm, &nalt);

  addSeparator(item);
  addTextItem(item, tr("Azimuth"), getStrDeg(azm));
  addTextItem(item, tr("Altitude"), getStrDeg(alt));
  addSeparator(item);
  addTextItem(item, tr("Altitude without ref."), getStrDeg(nalt));
  addTextItem(item, tr("Atm. refraction"), getStrDeg(cAstro.getAtmRef(nalt)));
  addSeparator(item);

  CRts   cRts;
  rts_t  rts;
  cRts.calcOrbitRTS(&rts, obj->par1, MO_PLANET, view);
  fillRTS(&rts, view, item);

  ra  = o.gRD.Ra;
  dec = o.gRD.Dec;

  if (view->epochJ2000 && view->coordType == SMCT_RA_DEC)
  {
    jd2000 = txJ2000;
    precess(&ra, &dec, view->jd, JD2000);
  }

  addLabelItem(item, tr("Geocentric information"));
  addSeparator(item);
  addTextItem(item, txRA + jd2000, getStrRA(ra));
  addTextItem(item, txDec + jd2000, getStrDeg(dec));
  if (item->par1 != PT_MOON)
  {
    addTextItem(item, tr("R"), QString::number(o.R) + tr("AU"));
    addTextItem(item, tr("Light time"), QString::number(o.light * 24.) + tr(" hours"));
  }
  else
  { // moon
    addTextItem(item, tr("Dist."), QString::number(o.r * EARTH_DIAM) + tr("Km"));
    addTextItem(item, tr("Light time"), QString::number(o.light * 24. * 3600.) + tr(" sec."));
  }

  addSeparator(item);

  if (item->par1 != PT_MOON)
  {
    addLabelItem(item, tr("Heliocentric information"));
    addSeparator(item);
    addTextItem(item, tr("X"), QString::number(o.hRect[0], 'f', 8));
    addTextItem(item, tr("Y"), QString::number(o.hRect[1], 'f', 8));
    addTextItem(item, tr("Z"), QString::number(o.hRect[2], 'f', 8));
    addSeparator(item);
    addTextItem(item, tr("Longitude"), QString::number(R2D(o.hLon), 'f', 8));
    addTextItem(item, tr("Latitude"), QString::number(R2D(o.hLat), 'f', 8));
    addSeparator(item);
    addTextItem(item, tr("r"), QString::number(o.r) + tr("AU"));
    addSeparator(item);
  }


  addLabelItem(item, tr("Apparent view"));
  addSeparator(item);

  if (o.sx == o.sy)
    diam = QString("%1\"").arg(o.sx, 0, 'f', 2);
  else
    diam = QString("%1\" x %2\"").arg(o.sx, 0, 'f', 2).arg(o.sy, 0, 'f', 2);

  addTextItem(item, tr("Apparent diameter"), diam);

  if (o.sx == o.sy)
  {
    QString str  = QString("%1\"").arg(o.sx * 0.5, 0, 'f', 2);
    addTextItem(item, tr("Apparent radius"), str);
  }

  if (o.type != PT_SUN)
  {
    addTextItem(item, tr("Phase"), QString("%1%").arg(o.phase * 100.0, 0, 'f', 2));
    addTextItem(item, tr("Phase angle"), QString("%1°").arg(R2D(o.FV), 0, 'f', 2));
  }
  addTextItem(item, tr("P.A."), QString("%1°").arg(R2D(o.PA), 0, 'f', 2));

  if (o.cMer != CM_UNDEF)
  {
    addTextItem(item, tr("Central Meridian"), QString("%1°").arg(R2D(o.cMer), 0, 'f', 2));
  }

  if (o.cLat != CM_UNDEF)
  {
    addTextItem(item, tr("Sub-Earth Phi"), QString("%1°").arg(R2D(o.cLat), 0, 'f', 2));
  }

  addSeparator(item);

  ra  = o.lRD.Ra;
  dec = o.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  addLabelItem(item, tr("Position at JD2000.0"));
  addSeparator(item);
  addTextItem(item, txRA, getStrRA(ra));
  addTextItem(item, txDec, getStrDeg(dec));
  addSeparator(item);

  fillAtlas(ra, dec, item);

  addLabelItem(item, tr("Source"));
  addSeparator(item);
  addTextItem(item, "S. L. Moshier", "Residuals against JPL ephemeris DE404");
}

//////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::fillESInfo(const mapView_t *view, const mapObj_t * /*obj*/, ofiItem_t *item)
//////////////////////////////////////////////////////////////////////////////////////////
{
  orbit_t o;
  orbit_t moon;

  cAstro.calcPlanet(PT_MOON, &moon);
  cAstro.calcEarthShadow(&o, &moon);

  double ra  = o.lRD.Ra;
  double dec = o.lRD.Dec;

  precess(&ra, &dec, view->jd, JD2000);

  item->radec.Ra = ra;
  item->radec.Dec = dec;
  item->zoomFov = getOptObjFov(o.sx / 3600., o.sy / 3600.);
  item->title = o.name;
  item->simbad = "";
  item->id = QString("PLN_ES");
  item->par1 = PT_EARTH_SHADOW;

  addLabelItem(item, txDateTime);
  addSeparator(item);
  addTextItem(item, tr("JD"), QString::number(view->jd, 'f'));
  addTextItem(item, tr("Date/Time"), QString("%1 / %2").arg(getStrDate(view->jd, view->geo.tz)).arg(getStrTime(view->jd, view->geo.tz)));
  addSeparator(item);

  addLabelItem(item, txObjType);
  addSeparator(item);
  addTextItem(item, txObjType, item->title);
  addSeparator(item);

  QString str  = QString("%1\"").arg(o.sy * 0.5, 0, 'f', 2);
  addTextItem(item, tr("Apparent umbra radius"), str);
  str  = QString("%1\"").arg(o.sx * 0.5, 0, 'f', 2);
  addTextItem(item, tr("Apparent penumbra radius"), str);

  addSeparator(item);

  str  = QString("%1\"").arg(o.sy, 0, 'f', 2);
  addTextItem(item, tr("Apparent umbra diameter"), str);
  str  = QString("%1\"").arg(o.sx, 0, 'f', 2);
  addTextItem(item, tr("Apparent penumbra diameter"), str);

}


/////////////////////////////////////////////////////////////////////////////////////////
void CObjFillInfo::addTextItem(ofiItem_t *item, QString label, QString value, bool bBold)
/////////////////////////////////////////////////////////////////////////////////////////
{
  ofiTextItem_t i;

  i.bIsTitle = false;
  i.label = label;
  i.value = value;
  i.bBold = bBold;

  item->tTextItem.append(i);
}


///////////////////////////////////////////////////////////////
void CObjFillInfo::addLabelItem(ofiItem_t *item, QString label)
///////////////////////////////////////////////////////////////
{
  ofiTextItem_t i;

  i.bIsTitle = true;
  i.label = label;
  i.bBold = true;

  item->tTextItem.append(i);
}


////////////////////////////////////////////////
void CObjFillInfo::addSeparator(ofiItem_t *item)
////////////////////////////////////////////////
{
  ofiTextItem_t i;

  i.bIsTitle = true;
  i.bBold = false;
  item->tTextItem.append(i);
}


