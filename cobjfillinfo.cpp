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
  addTextItem(item, tr("Milenium Star Atlas"), tmpStr);
  getStarAtlas2000(tmpStr, ra, dec);
  addTextItem(item, tr("Sky Atlas 2000.0"), tmpStr);
  getUranometriaAtlas2000(tmpStr, ra, dec);
  addTextItem(item, tr("Uranometria 2000"), tmpStr);

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
      break;

    case RTS_CIRC:
      {
        addTextItem(item, tr("Object is circumpolar."), "");
        addTextItem(item, tr("Transit"), getStrTime(rts->transit, tz) + " " + tr("Alt : ") + getStrDegDF(rts->tAlt));
      }
      break;

    case RTS_NONV:
      addTextItem(item, tr("Object is never visible!"), "");
      addTextItem(item, tr("Transit"), getStrTime(rts->transit, tz) + " " + tr("Alt : ") + getStrDegDF(rts->tAlt));
      break;

    case RTS_DONE:
      if ((rts->rts & RTS_T_RISE) == RTS_T_RISE)
        addTextItem(item, tr("Rise"), getStrTime(rts->rise, tz) + " " + tr("Azm : ") + getStrDegDF(rts->rAzm));
      if ((rts->rts & RTS_T_TRANSIT) == RTS_T_TRANSIT)
        addTextItem(item, tr("Transit"), getStrTime(rts->transit, tz) + " " + tr("Alt : ") + getStrDegDF(rts->tAlt));
      if ((rts->rts & RTS_T_SET) == RTS_T_SET)
        addTextItem(item, tr("Set"), getStrTime(rts->set, tz) + " " + tr("Azm : ") + getStrDegDF(rts->sAzm));
      break;
  }
  addSeparator(item);
}

#define SAT_VIS_NONE     0  //   /*!< Unknown/undefined. */
#define SAT_VIS_VISIBLE  1  //    /*!< Visible. */
#define SAT_VIS_DAYLIGHT 2  //    /*!< Satellite is in daylight. */
#define SAT_VIS_ECLIPSED 3  //    /*!< Satellite is eclipsed. */

#define secday   8.6400E4        /* Seconds per day */
#define de2ra    1.74532925E-2   /* Degrees to Radians */
#define twopi    6.2831853071796 /* 2*Pi  */
#define AU       1.49597870E8   /*Astronomical unit - kilometers (IAU 76)*/
#define pio2     1.5707963267949 /* Pi/2 */
#define xkmper   6.378135E3      /* Earth radius km */
#define __sr__   6.96000E5      /*Solar radius - kilometers (IAU 76)*/

/* Returns arg1 mod arg2 */
double Modulus(double arg1, double arg2)
{
  int i;
  double ret_val;

  ret_val = arg1;
  i = ret_val/arg2;
  ret_val -= i*arg2;
  if (ret_val < 0) ret_val += arg2;

  return (ret_val);
} /* modulus */

double Radians(double arg)
{
  return( arg*de2ra );
} /*Function Radians*/

double Sqr(double arg)
{
  return( arg*arg );
} /* Function Sqr */

double Delta_ET(double year)
{
  /* Values determined using data from 1950-1991 in the 1990
     Astronomical Almanac.  See DELTA_ET.WQ1 for details. */

  double delta_et;

  delta_et = 26.465 + 0.747622*(year - 1950) +
             1.886913*sin(twopi*(year - 1975)/33);

  return( delta_et );
} /*Function Delta_ET*/

int Sign(double arg)
{
  if( arg > 0 )
    return( 1 );
  else if( arg < 0 )
    return( -1 );
  else
    return( 0 );
} /* Function Sign*/

/* Returns the arcsine of the argument */
double ArcSin(double arg)
{
  if( fabs(arg) >= 1 )
    return( Sign(arg)*pio2 );
  else
    return( atan(arg/sqrt(1-arg*arg)) );
} /*Function ArcSin*/

/* Calculates scalar magnitude of a vector_t argument */
void Magnitude(double *v)
{
  v[3] = sqrt(Sqr(v[0]) + Sqr(v[1]) + Sqr(v[2]));
} /*Procedure Magnitude*/

/* Subtracts vector v2 from v1 to produce v3 */
void Vec_Sub(double *v1, double *v2, double *v3)
{
  v3[0] = v1[0] - v2[0];
  v3[1] = v1[1] - v2[1];
  v3[2] = v1[2] - v2[2];

  Magnitude(v3);
} /*Procedure Vec_Sub*/


void Calculate_Solar_Position(double _time, double *solar_vector)
{
  double mjd,year,T,M,L,e,C,O,Lsa,nu,R,eps;

  mjd = _time - 2415020.0;
  year = 1900 + mjd/365.25;
  T = (mjd + Delta_ET(year)/secday)/36525.0;
  M = Radians(Modulus(358.47583 + Modulus(35999.04975*T,360.0)
          - (0.000150 + 0.0000033*T)*Sqr(T),360.0));
  L = Radians(Modulus(279.69668 + Modulus(36000.76892*T,360.0)
          + 0.0003025*Sqr(T),360.0));
  e = 0.01675104 - (0.0000418 + 0.000000126*T)*T;
  C = Radians((1.919460 - (0.004789 + 0.000014*T)*T)*sin(M)
        + (0.020094 - 0.000100*T)*sin(2*M) + 0.000293*sin(3*M));
  O = Radians(Modulus(259.18 - 1934.142*T,360.0));
  Lsa = Modulus(L + C - Radians(0.00569 - 0.00479*sin(O)),twopi);
  nu = Modulus(M + C,twopi);
  R = 1.0000002*(1 - Sqr(e))/(1 + e*cos(nu));
  eps = Radians(23.452294 - (0.0130125 + (0.00000164 -
    0.000000503*T)*T)*T + 0.00256*cos(O));
  //R = AU*R;
  solar_vector[0] = R*cos(Lsa);
  solar_vector[1] = R*sin(Lsa)*cos(eps);
  solar_vector[2] = R*sin(Lsa)*sin(eps);
  solar_vector[3] = R;
} /*Procedure Calculate_Solar_Position*/

/* Returns the dot product of two vectors */
double Dot(double *v1, double *v2)
{
  return( v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}  /*Function Dot*/

/* Returns orccosine of rgument */
double ArcCos(double arg)
{
  return( pio2 - ArcSin(arg) );
} /*Function ArcCos*/

/* Calculates the angle between vectors v1 and v2 */
double Angle(double  *v1, double *v2)
{
  Magnitude(v1);
  Magnitude(v2);
  return( ArcCos(Dot(v1,v2)/(v1[3]*v2[3])) );
} /*Function Angle*/

/* Multiplies the vector v1 by the scalar k to produce the vector v2 */
void Scalar_Multiply(double k, double *v1, double *v2)
{
  v2[0] = k * v1[0];
  v2[1] = k * v1[1];
  v2[2] = k * v1[2];
  v2[3] = fabs(k) * v1[3];
} /*Procedure Scalar_Multiply*/

/* Calculates stellite's eclipse status and depth */
int Sat_Eclipsed(double *pos, double *sol, double *depth)
{
  double sd_sun, sd_earth, delta;
  double Rho[4], earth[4];

  /* Determine partial eclipse */
  sd_earth = ArcSin(xkmper/pos[3]);
  Vec_Sub(sol,pos, Rho);
  sd_sun = ArcSin(__sr__ / Rho[3]);
  Scalar_Multiply(-1,pos, earth);
  delta = Angle(sol, earth);
  *depth = sd_earth - sd_sun - delta;
  if( sd_earth < sd_sun )
    return( 0 );
  else
    if( *depth >= 0 )
      return( 1 );
    else
      return( 0 );

} /*Function Sat_Eclipsed*/

static int vis(satellite_t *sat, const mapView_t *view, const mapObj_t *obj, ofiItem_t *item)
{
  bool sat_sun_status;
  double  sun_el;
  double  threshold;
  double  eclipse_depth;
  int visibility = SAT_VIS_NONE;
  double zero_vector[4] = {0,0,0,0};
  double obs_geodetic[4];

  double solar_vector[4] = { zero_vector[0], zero_vector[1], zero_vector[2], zero_vector[3]};

  /* Solar observed az and el vector  */
  //obs_set_t solar_set;

  /* FIXME: could be passed as parameter */
  obs_geodetic[0] = view->geo.lon;
  obs_geodetic[1] = view->geo.lat;
  obs_geodetic[2] = view->geo.alt / 1000.0;// qth->alt / 1000.0;
  obs_geodetic[4] = 0;  // theta

  qDebug() << view->geo.alt;

  Calculate_Solar_Position (view->jd, solar_vector);
  //Calculate_Obs (jul_utc, &solar_vector, &zero_vector, &obs_geodetic, &solar_set);

  orbit_t sun;
  mapView_t newView = *view;

  cAstro.setParam(&newView);
  cAstro.calcPlanet(PT_SUN, &sun);

  double wsat[4];
  wsat[0] = sat->longitude;
  wsat[1] = sat->latitude;
  wsat[2] = sat->altitude;
  wsat[3] = 0;

  if (Sat_Eclipsed(wsat, solar_vector, &eclipse_depth))
  {
         /* satellite is eclipsed */
         sat_sun_status = FALSE;
     }
     else {
         /* satellite in sunlight => may be visible */
         sat_sun_status = TRUE;
     }

  if (sat_sun_status)
  {
          sun_el = R2D(sun.lAlt);//solar_set.el);
          threshold = -6;//(gdouble) sat_cfg_get_int (SAT_CFG_INT_PRED_TWILIGHT_THLD);

          if (sun_el <= threshold && sat->elevation >= 0.0)
              visibility = SAT_VIS_VISIBLE;
          else
              visibility = SAT_VIS_DAYLIGHT;
      }
      else
          visibility = SAT_VIS_ECLIPSED;


  return visibility;
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

  str = str.remove(QRegExp("/\\?%*:|\"<>. "));
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

  str = str.remove(QRegExp("/\\?%*:|\"<>. "));
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

  // TODO rise set transit button
  //item->riseJD = rts.rise;

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

  str = str.remove(QRegExp("/\\?%*:|\"<>. "));
  item->id = "ast_" + str;

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
  item->id = s.name;
  item->title = s.name;
  item->simbad = item->id;

  addTextItem(item, "Visibility", QString::number(vis(&s, view, obj, item)));

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
  ucac4Region_t *z;

  z = cUcac4.getStar(s, obj->par1, obj->par2);

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


