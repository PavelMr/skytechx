#include "QtCore"
#include "csatxyz.h"
#include "castro.h"

#define	SCANFLD(f, w, vp)	if(readField(fp, f, w,(void *)vp) < 0) return (-1)

CSatXYZ       cSatXYZ;

CSatXYZ::CSatXYZ()
{
  bOk = false;
}

CSatXYZ::~CSatXYZ()
{
}

////////////////////
bool CSatXYZ::init()
////////////////////
{
  QString names[8] = {"mars.9910", "mars.1020",
                      "jupiter.9910", "jupiter.1020",
                      "saturn.9910", "saturn.1020",
                      "uranus.9910", "uranus.1020"};

  for (int i = 0; i < 8; i++)
  {
    SkFile f("data/posxyz/" + names[i]);

    if (f.open(SkFile::ReadOnly))
    {
      array[i] = f.readAll();
      f.close();
    }
    else
    {
      for (int i = 0; i < 8; i++)
        array[i].clear();
      bOk = false;
      return(false);
    }
  }

  bOk = true;
  return(true);
}

/* read one field.
 * return 0 if ok else -1
 * N.B. this is enforce width, without skipping leading blanks.
 */
/////////////////////////////////////////////////////////////////////////
int CSatXYZ::readField(QDataStream *fp, ScanType f, int width, void *ptr)
/////////////////////////////////////////////////////////////////////////
{
  char buf[128];
  char *bp;

  if (width > (int)sizeof(buf) - 1)
  {
    //sprintf (ynot, "BDL Field width %d > %d", width, (int)sizeof(buf));
    return (-1);
  }
  if (width != (int)fp->readRawData(buf, width))
  {
    //if (ferror(fp)) strcpy (ynot, "BDL IO error");
    //else if (feof(fp)) strcpy (ynot, "BDL unexpected EOF");
    //else strcpy (ynot, "BDL short file");
    return (-1);
  }

  buf[width] = '\0';
  switch (f)
  {
    case I:
        *(int *)ptr = atoi (buf);
        break;
    case F:
        bp = strchr(buf, 'D');
        if (bp)
      *bp = 'e';
        *(double *)ptr = atof (buf);
        break;
    case NL:
        //fgets(buf, sizeof(buf), fp);
        while (1)
        {
          char c;
          fp->readRawData(&c, 1);
          if (c == '\n')
            break;
        }
        break;
    default:
        //sprintf (ynot, "Bug! format = %d", f);
        return (-1);
  }
  return (0);
}



/* read one satellite record.
 * return number of chars read else -1.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
qint64 CSatXYZ::readRec(QDataStream *fp, double *t0, double cmx[], double cfx[], double cmy[], double cfy[], double cmz[], double cfz[])
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{

  qint64 pos0, pos1;
  int isat, idx;
  int ldat1, ldat2;
  int i;

  pos0 = fp->device()->pos();

  SCANFLD (I, 1, &isat);
  SCANFLD (I, 5, &idx);
  SCANFLD (I, 8, &ldat1);
  SCANFLD (I, 8, &ldat2);
  SCANFLD (F, 9, t0);
  for (i = 0; i < 6; i++)
      SCANFLD (F, 17, &cmx[i]);
  for (i = 0; i < 4; i++)
      SCANFLD (F, 17, &cfx[i]);
  for (i = 0; i < 6; i++)
      SCANFLD (F, 17, &cmy[i]);
  for (i = 0; i < 4; i++)
      SCANFLD (F, 17, &cfy[i]);
  for (i = 0; i < 6; i++)
      SCANFLD (F, 17, &cmz[i]);
  for (i = 0; i < 4; i++)
      SCANFLD (F, 17, &cfz[i]);
  SCANFLD (NL, 0, NULL);

  pos1 = fp->device()->pos();

  return (pos1 - pos0);
}


/* given a sequencial text file in BDL natural satellite ephemeris format and a
 * JD, find the x/y/z positions of each satellite. store in the given arrays,
 * assumed to have one entry per moon. values are planetocentric, +x east, +y
 * north, +z away from earth, all in au. corrected for light time.
 * return the number of satellites or -1 and reason in ymot[].
 * files obtained from ftp://ftp.bdl.fr/pub/misc/satxyz.
 */
/////////////////////////////////////////////////////////////////////////////////////
int CSatXYZ::read_bdl(QByteArray *fpa, double jd, double *xp, double *yp, double *zp)
/////////////////////////////////////////////////////////////////////////////////////
{
  int npla;
  int nsat;
  int idn[8];
  double freq[8];
  double delt[8];
  int ienrf;
  double djj;
  int jan;
  qint64 os0;
  int reclen;
  double t0;
  double cmx[6], cfx[4], cmy[6], cfy[4], cmz[6], cfz[4];

  QDataStream fpds(fpa, QIODevice::ReadOnly);
  QDataStream *fp = &fpds;

  /* read header line */
  SCANFLD (I, 2, &npla);
  SCANFLD (I, 2, &nsat);
  for (int i = 0; i < nsat; i++)
      SCANFLD (I, 5, &idn[i]);
  for (int i = 0; i < nsat; i++)
      SCANFLD (F, 8, &freq[i]);
  for (int i = 0; i < nsat; i++)
      SCANFLD (F, 5, &delt[i]);
  SCANFLD (I, 5, &ienrf);
  SCANFLD (F, 15, &djj);
  SCANFLD (I, 5, &jan);
  SCANFLD (NL, 0, NULL);

  os0 = fp->device()->pos();

  /* read first record to get length */
  reclen = readRec(fp, &t0, cmx, cfx, cmy, cfy, cmz, cfz);
  if (reclen < 0)
    return (-1);

  /* compute location of each satellite */
  for (int i = 0; i < nsat; i++)
  {
    int id = (int)floor((jd-djj)/delt[i]) + idn[i] - 2;
    qint64 os = os0 + id*reclen;
    double t1, anu, tau, tau2, at;
    double tbx, tby, tbz;

    if (!fp->device()->seek(os))
    {
      //sprintf (ynot, "Seek error to %ld for rec %d", os, id);
      return (-1);
    }

    if (readRec (fp, &t0, cmx, cfx, cmy, cfy, cmz, cfz) < 0)
      return (-1);

    t1 = floor(t0) + 0.5;
    anu = freq[i];
    tau = jd - t1;
    tau2 = tau * tau;
    at = tau*anu;

    tbx = cmx[0]+cmx[1]*tau+cmx[2]*sin(at+cfx[0])
         +cmx[3]*tau*sin(at+cfx[1])
         +cmx[4]*tau2*sin(at+cfx[2])
         +cmx[5]*sin(2*at+cfx[3]);
    tby = cmy[0]+cmy[1]*tau+cmy[2]*sin(at+cfy[0])
         +cmy[3]*tau*sin(at+cfy[1])
         +cmy[4]*tau2*sin(at+cfy[2])
         +cmy[5]*sin(2*at+cfy[3]);
    tbz = cmz[0]+cmz[1]*tau+cmz[2]*sin(at+cfz[0])
         +cmz[3]*tau*sin(at+cfz[1])
         +cmz[4]*tau2*sin(at+cfz[2])
         +cmz[5]*sin(2*at+cfz[3]);

    xp[i] = tbx * 1000. / 149597870.;
    yp[i] = tby * 1000. / 149597870.;
    zp[i] = tbz * 1000. / 149597870.;
  }

  return(nsat);
}


////////////////////////////////////////////////////////////////////////////////
bool CSatXYZ::solve(double jd, int pln, orbit_t *o, orbit_t *sun, satxyz_t *sat)
////////////////////////////////////////////////////////////////////////////////
{
  QByteArray *plArray;
  double x[MAX_XYZ_SATS];
  double y[MAX_XYZ_SATS];
  double z[MAX_XYZ_SATS];
  double div;
  double flat = o->sx / o->sy;

  sat->count = 0;

  if (pln == PT_MARS)
  {
    plArray = &array[0];
    div = .00002269;     /* Mars radius, AU */
  }
  else
  if (pln == PT_JUPITER)
  {
    plArray = &array[2];
    div = .0004769108;	/* jupiter radius, AU */
  }
  else
  if (pln == PT_SATURN)
  {
    plArray = &array[4];
    div = .0004014253;	/* saturn radius, AU */

  }
  else
  if (pln == PT_URANUS)
  {
    plArray = &array[6];
    div = .0001597;     /* Uranus radius, AU */
  }
  else
    return(false);

  /* check ranges and appropriate data file */
  if (jd < 2451179.50000)		/* Jan 1 1999 UTC */
    return (false);
  if (jd < 2455562.5)		/* Jan 1 2011 UTC */
    plArray += 0;
  else if (jd < 2459215.5)	/* Jan 1 2021 UTC */
    plArray += 1;
  else
    return (false);

  if (plArray->count() == 0)
    return(false);

  int c = read_bdl(plArray, jd, x, y, z);

  sat->count = c;

  for (int i = 0; i < c; i++)
  {
    sat->sat[i].x =  x[i] / div;
    sat->sat[i].y = -y[i] / div;
    sat->sat[i].z = -z[i] / div;
  }

  switch (pln)
  {
    case PT_MARS:
    {
      if (c != 2) return(false);

      float dmag = 5.0 * log10(o->r + 0.4);

      sat->sat[0].name = "Phobos I";
      sat->sat[1].name = "Deimos II";

      sat->sat[0].mag = 11.8 + dmag;
      sat->sat[1].mag = 12.9 + dmag;

      sat->sat[0].diam = 22.2; // km
      sat->sat[1].diam = 12.4; // km
    }
    break;

    case PT_JUPITER:
    {
      if (c != 4) return(false);

      sat->sat[0].name = "Io I";
      sat->sat[1].name = "Europa II";
      sat->sat[2].name = "Ganymede III";
      sat->sat[3].name = "Callisto IV";

      sat->sat[0].mag = 5.7f;
      sat->sat[1].mag = 5.8f;
      sat->sat[2].mag = 5.3f;
      sat->sat[3].mag = 6.7f;

      sat->sat[0].diam = 3630.6; // km
      sat->sat[1].diam = 3121.6; // km
      sat->sat[2].diam = 5262.4; // km
      sat->sat[3].diam = 4820.6; // km
    }
    break;

    case PT_SATURN:
    {
      if (c != 8) return(false);

      sat->sat[0].name = "Mimas I";
      sat->sat[1].name = "Enceladus II";
      sat->sat[2].name = "Tethys III";
      sat->sat[3].name = "Dione IV";
      sat->sat[4].name = "Rhea V";
      sat->sat[5].name = "Titan VI";
      sat->sat[6].name = "Hyperion VII";
      sat->sat[7].name = "Iapetus VIII";

      sat->sat[0].mag = 13.0f;
      sat->sat[1].mag = 11.8f;
      sat->sat[2].mag = 10.3f;
      sat->sat[3].mag = 10.2f;
      sat->sat[4].mag = 9.8f;
      sat->sat[5].mag = 8.4f;
      sat->sat[6].mag = 14.3f;
      sat->sat[7].mag = 11.2f;

      sat->sat[0].diam = 396; // km
      sat->sat[1].diam = 504; // km
      sat->sat[2].diam = 1062; // km
      sat->sat[3].diam = 1123; // km
      sat->sat[4].diam = 1527; // km
      sat->sat[5].diam = 5150; // km
      sat->sat[6].diam = 135; // km
      sat->sat[7].diam = 1470; // km
    }
    break;

    case PT_URANUS:
    {
      if (c != 5) return(false);

      sat->sat[0].name = "Ariel I";
      sat->sat[1].name = "Umbriel II";
      sat->sat[2].name = "Titania III";
      sat->sat[3].name = "Oberone IV";
      sat->sat[4].name = "Miranda V";

      sat->sat[0].mag = 14.2f;
      sat->sat[1].mag = 14.8f;
      sat->sat[2].mag = 13.7f;
      sat->sat[3].mag = 14.0f;
      sat->sat[4].mag = 16.3f;

      sat->sat[0].diam = 1157.8; // km
      sat->sat[1].diam = 1169.4; // km
      sat->sat[2].diam = 1576.8; // km
      sat->sat[3].diam = 1522.8; // km
      sat->sat[4].diam = 471.6; // km
    }
    break;
  }

  for (int i = 0; i < c; i++)
  {
    sat->sat[i].inFront = sat->sat[i].z > 0;
    sat->sat[i].size = cAstro.calcAparentSize(o->R, sat->sat[i].diam);
    sat->sat[i].distanceFromPlanet = sqrt(sat->sat[i].x * sat->sat[i].x +
                                          sat->sat[i].y * sat->sat[i].y +
                                          sat->sat[i].z * sat->sat[i].z) * o->dx * 0.5;
    if (o->type == PT_JUPITER)
    {
      qDebug() << "pln" << sat->sat[i].name << sat->sat[i].distanceFromPlanet;
    }

    double rad = DEG2RAD((double)o->sx / 3600.0) / 2.0;
    double ra = o->lRD.Ra;
    double dec = o->lRD.Dec;
    double dra  = rad * sat->sat[i].x;
    double ddec = rad * sat->sat[i].y;

    sat->sat[i].rd.Ra  = ra + dra;
    sat->sat[i].rd.Dec = dec - ddec;

    moonSVis(o, sun, &sat->sat[i]);
    sat->sat[i].throwShadow = plshadow(o, sun, o->poleRa, o->poleDec, sat->sat[i].x, sat->sat[i].y, sat->sat[i].z, &sat->sat[i].sx, &sat->sat[i].sy);

    if (sat->sat[i].throwShadow)
    {
      double sra  = rad * sat->sat[i].sx;
      double sdec = rad * sat->sat[i].sy;
      sat->sat[i].srd.Ra  = ra + sra;
      sat->sat[i].srd.Dec = dec - sdec;
      calcShadowSize(&sat->sat[i], sun);
    }

    int outside = POW2(sat->sat[i].x) + POW2(sat->sat[i].y * flat) > 1.0;
    int infront = sat->sat[i].z > 0.0;
    sat->sat[i].isHidden = !(outside || infront);
    sat->sat[i].isTransit = (infront && !outside);

    sat->sat[i].distance = anSep(o->lRD.Ra, o->lRD.Dec, sat->sat[i].rd.Ra, sat->sat[i].rd.Dec);
  }
  return(true);
}


/* given a planet, the sun, the planet's eq pole position and a
 * position of a satellite (as eq x=+e y=+s z=front in planet radii) find x,y
 * position of shadow.
 * return true if ok else false if shadow not on planet
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CSatXYZ::plshadow(orbit_t *op, orbit_t *sop, double polera, double poledec, double x, double y, double z, float *sxp, float *syp)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  double flat = op->sx / op->sy;

  /* equatorial to ecliptic sky-plane rotation */
  double sa = cos(op->lRD.Dec) * cos(poledec) *
             (cos(op->lRD.Ra)*sin(polera) - sin(op->lRD.Ra)*cos(polera));
  double ca = sqrt (1.0 - sa * sa);

  /* rotate moon from equatorial to ecliptic */
  double ex =  x*ca + y*sa*flat;
  double ey = -x*sa + y*ca*flat;

  /* find angle subtended by earth-sun from planet */
  double a = asin (sin(op->hLon - (sop->hLon - R180)) / op->R);
  double b = asin (-sin(op->hLat)/op->R);

  /* find displacement in sky plane */
  double x0 = ex - z*tan(a);
  double y0 = ey - z*tan(b);

  /* projection onto unit sphere */
  double x1 = x0 + (ex-x0)/sqrt(POW2(ex-x0)+POW2(z));
  double y1 = y0 + (ey-y0)/sqrt(POW2(ey-y0)+POW2(z));

  /* check behind or off edge */
  if (z < 0 || POW2(x1) + POW2(y1) > 1)
    return (false);

  /* rotate back to equatorial */
  *sxp = x1*ca - y1*sa;
  *syp = x1*sa + y1*ca;

  return (true);
}

void CSatXYZ::calcShadowSize(sat_t *sat, orbit_t *sun)
{
  double ERAD = sat->diam;
  double erad = ERAD;
  double SRAD = 696010000;
  double jsd = sqrt(POW2(sat->x) + POW2(sat->y) + POW2(sat->z));// jupiter - satellite distance

  // TODO: dopocitat a pak pouzit v planetrendereru
  jsd *= 1;

  double mr = jsd * ERAD;

  double s1;
  double s2;

  s1 = erad + mr / (sun->r * AU1 * 1000) * (SRAD - erad);
  s2 = erad - mr / (sun->r * AU1 * 1000) * (SRAD - erad);

  s1 = atan2(s1, mr) * 2;
  s2 = atan2(s2, mr) * 2;

  s1 = RAD2DEG(3600 * s1);
  s2 = RAD2DEG(3600 * s2);
}

//////////////////////////////////////////////////////////
void CSatXYZ::moonSVis(orbit_t *o, orbit_t *s, sat_t *sat)
//////////////////////////////////////////////////////////
{
  double esd = s->R;
  double eod = o->R;
  double sod = o->r;
  double soa = o->elongation;
  double esa = asin(esd * sin(soa) / sod);
  double   h = sod * o->hLat;
  double nod = h * (1. / eod - 1. / sod);
  double sca = cos(esa);
  double ssa = sin(esa);

  double xp =  sca * sat->x + ssa * sat->z;
  double yp =  sat->y;
  double zp = -ssa * sat->x + sca * sat->z;
  double ca = cos(nod), sa = sin(nod);
  double xpp = xp;
  double ypp = ca * yp - sa * zp;
  double zpp = sa * yp + ca * zp;

  int outside = xpp * xpp + ypp * ypp > 1.0;
  int infront = zpp > 0.0;

  sat->inSunLgt = outside || infront;
}

