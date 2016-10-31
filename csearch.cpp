#include "csearch.h"
#include "casterdlg.h"
#include "ccomedit.h"
#include "clunarfeatures.h"
#include "csgp4.h"
#include "Gsc.h"
#include "Usno2A.h"
#include "cucac4.h"
#include "tycho.h"
#include "setting.h"
#include "cmeteorshower.h"
#include "gcvs.h"

//////////////////
CSearch::CSearch()
//////////////////
{
}

bool CSearch::search(mapView_t *mapView, QString str, double &ra, double &dec, double &fov, mapObj_t &obj)
{
  double yr = jdGetYearFromJD(mapView->mapEpoch) - 2000;
  QString what = str.mid(0, 4);
  str = str.mid(4);

  QRegExp reg("\\b" + str + "\\b", Qt::CaseInsensitive);

  str = str.simplified();

  cAstro.setParam(mapView);

  if (SS_CHECK_OR(SS_PLANET, what))
  {
    if (!str.compare("es", Qt::CaseInsensitive))
    {
      orbit_t o, m;

      cAstro.calcPlanet(PT_MOON, &m);
      cAstro.calcEarthShadow(&o, &m);

      ra = o.lRD.Ra;
      dec = o.lRD.Dec;
      fov = getOptObjFov(o.sx / 3600.0, o.sy / 3600.0);

      obj.type = MO_EARTH_SHD;

      return(true);
    }
  }

  if (SS_CHECK_OR(SS_STAR, what))
  {        
    if (str.startsWith("HD", Qt::CaseInsensitive))
    {
      str = str.mid(2);

      int hd = str.toInt();

      int reg, index;
      tychoStar_t *star;

      if (cTYC.findStar(NULL, TS_HD, 0, hd, 0, 0, 0, 0, 0, 0, reg, index))
      {
        cTYC.getStar(&star, reg, index);

        radec_t rdpm;
        cTYC.getStarPos(rdpm, star, yr);

        ra = rdpm.Ra;
        dec = rdpm.Dec;

        precess(&ra, &dec, JD2000, mapView->jd);
        fov = DMS2RAD(10, 0, 0);

        obj.type = MO_TYCSTAR;
        obj.par1 = reg;
        obj.par2 = index;

        return true;
      }
    }

    if (str.startsWith("TYC", Qt::CaseInsensitive))
    {
      str = str.mid(3);
      QStringList list = str.split("-");

      if (list.count() == 3)
      {
        int t1 = list[0].toInt();
        int t2 = list[1].toInt();
        int t3 = list[2].toInt();
        int reg, index;
        tychoStar_t *star;

        if (cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, t1, t2, t3, 0, reg, index))
        {
          cTYC.getStar(&star, reg, index);

          radec_t rdpm;
          cTYC.getStarPos(rdpm, star, yr);

          ra = rdpm.Ra;
          dec = rdpm.Dec;

          precess(&ra, &dec, JD2000, mapView->jd);
          fov = DMS2RAD(10, 0, 0);

          obj.type = MO_TYCSTAR;
          obj.par1 = reg;
          obj.par2 = index;

          return true;
        }
      }
    }

    if (str.startsWith("UCAC4", Qt::CaseInsensitive))
    {
      str = str.mid(5);
      QStringList list = str.split("-");

      if (list.count() == 2)
      {
        int zone = list[0].toInt();
        int num = list[1].toInt();
        ucac4Star_t star;

        if (cUcac4.searchStar(zone, num, &star))
        {
          radec_t rdpm;
          cUcac4.getStarPos(rdpm, star, yr);

          ra = rdpm.Ra;
          dec = rdpm.Dec;

          precess(&ra, &dec, JD2000, mapView->jd);
          fov = DMS2RAD(0, 30, 0);

           // FIXME: region a poradi v GSC regionu
          /*
          obj.type = MO_UCAC4;
          obj.par1 = star.zone;
          obj.par2 = star.number;
          */

          return true;
        }
      }
      return false;
    }

    if (str.startsWith("USNO2", Qt::CaseInsensitive))
    {
      str = str.mid(5);
      QStringList list = str.split("-");

      if (list.count() == 2)
      {
        int zone = list[0].toInt();
        int num = list[1].toInt();
        usnoStar_t star;

        if (usno.searchStar(zone, num, &star))
        {
          ra = star.rd.Ra;
          dec = star.rd.Dec;
          precess(&ra, &dec, JD2000, mapView->jd);
          fov = DMS2RAD(0, 30, 0);

          // FIXME: region a poradi v GSC regionu
          /*
          obj.type = MO_USNO2;
          obj.par1 = star.zone;
          obj.par2 = star.number;
          */

          return true;
        }
      }
      return false;
    }

    if (str.startsWith("GSC", Qt::CaseInsensitive))
    {
      str = str.mid(3);
      QStringList list = str.split("-");

      if (list.count() == 2)
      {
        int reg = list[0].toInt();
        int num = list[1].toInt();
        int index;
        gsc_t *star;

        if (cGSC.searchStar(reg, num, &star, index))
        {
          ra = star->Ra;
          dec = star->Dec;
          precess(&ra, &dec, JD2000, mapView->jd);
          fov = DMS2RAD(0, 30, 0);

          obj.type = MO_GSCSTAR;
          obj.par1 = reg - 1;
          obj.par2 = index;

          return true;
        }
      }
      return false;
    }

    gcvs_t gcvs;
    if (g_GCVS.findStar(str, &gcvs))
    {
      int reg, index;
      tychoStar_t *star;

      if (cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, gcvs.tyc1, gcvs.tyc2, gcvs.tyc3, 0, reg, index))
      {
        cTYC.getStar(&star, reg, index);

        radec_t rdpm;
        cTYC.getStarPos(rdpm, star, yr);

        ra = rdpm.Ra;
        dec = rdpm.Dec;

        precess(&ra, &dec, JD2000, mapView->jd);
        fov = DMS2RAD(10, 0, 0);

        obj.type = MO_TYCSTAR;
        obj.par1 = reg;
        obj.par2 = index;

        return true;
      }
    }
  }

  if (SS_CHECK_OR(SS_POS, what))
  {
    // ra/dec
    {
      QStringList list = str.split(' ');
      if (list.count() == 6)
      {
        double rah, ram, ras;
        double decd, decm, decs;
        bool ok;

        for (int i = 0; i < 6; i++)
        {
          list.at(0).toDouble(&ok);
          if (!ok)
          {
            break;
          }
        }

        if (ok)
        {
          rah = list.at(0).toDouble();
          ram = list.at(1).toDouble();
          ras = list.at(2).toDouble();

          decd = list.at(3).toDouble();
          decm = list.at(4).toDouble();
          decs = list.at(5).toDouble();

          double mra = HMS2RAD(qAbs(rah), qAbs(ram), qAbs(ras));
          double mdec = DMS2RAD(qAbs(decd), qAbs(decm), qAbs(decs));

          if (decd < 0)
          {
            mdec = -mdec;
          }

          if (mra >= 0 && mra <= R360 && mdec >= -R90 && mdec <= R90)
          {
            ra = mra;
            dec = mdec;
            fov = CM_UNDEF;

            if (mapView->epochJ2000 && mapView->coordType == SMCT_RA_DEC)
            {
              precess(&ra, &dec, JD2000, mapView->jd);
            }

            return true;
          }
        }
      }
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_CONSTEL, what))
  {
    // constellation
    if (constFind(str, ra, dec, fov, mapView->jd))
      return(true);
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_STAR_NAME, what))
  {
    // star names
    for (int i = 0; i < cTYC.tNames.count(); i++)
    {
      int offs = cTYC.tNames[i]->supIndex;
      QString name = cTYC.getStarName(&cTYC.pSupplement[offs]);

      if (!str.compare(name, Qt::CaseInsensitive))
      {        
        int reg, index;

        if (cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, cTYC.tNames[i]->tyc1, cTYC.tNames[i]->tyc2, cTYC.tNames[i]->tyc3, 0, reg, index))
        {          
          tychoStar_t *star;
          radec_t rdpm;

          cTYC.getStar(&star, reg, index);
          cTYC.getStarPos(rdpm, star, yr);

          ra = rdpm.Ra;
          dec = rdpm.Dec;

          precess(&ra, &dec, JD2000, mapView->jd);
          fov = D2R(30);

          obj.type = MO_TYCSTAR;
          obj.par1 = reg;
          obj.par2 = index;

          return(true);
        }        
      }
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_PLANET, what))
  {
    // search planet/sun/moon
    for (int i = PT_SUN; i <= PT_MOON; i++)
    {
      orbit_t o;
      cAstro.setParam(mapView);
      cAstro.calcPlanet(i, &o);
      QString name = o.name;
      QString english = o.englishName;
      if (!str.compare(name, Qt::CaseInsensitive) ||
          !str.compare(english, Qt::CaseInsensitive))
      {
        ra = o.lRD.Ra;
        dec = o.lRD.Dec;
        fov = getOptObjFov(o.sx / 3600.0, o.sy / 3600.0);

        obj.type = MO_PLANET;
        obj.par1 = i;
        obj.par2 = 0;

        return(true);
      }
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_DSO, what))
  {
    // dso
    dso_t *dso;
    int index;
    if (cDSO.findDSO((char *)qPrintable(str), &dso, index) != -1)
    {
      ra = dso->rd.Ra;
      dec = dso->rd.Dec;
      precess(&ra, &dec, JD2000, mapView->jd);
      fov = getOptObjFov(dso->sx / 3600., dso->sy / 3600.);

      obj.type = MO_DSO;
      obj.par1 = (qint64)dso;
      obj.par2 = 0;

      return(true);
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_ART_SAT, what))
  {
    // satellites
    QString satName = str;

    sgp4.setObserver(mapView);

    for (int i = 0; i < sgp4.count(); i++)
    {
      satellite_t out;
      radec_t rd;

      if (sgp4.getName(i).compare(satName, Qt::CaseInsensitive) == 0)
      {
        if (sgp4.tleItem(i)->used && sgp4.solve(i, mapView, &out))
        {
          cAstro.convAA2RDRef(out.azimuth, out.elevation, &rd.Ra, &rd.Dec);

          ra = rd.Ra;
          dec = rd.Dec;
          fov = getOptObjFov(0, 0, D2R(2.5));

          obj.type = MO_SATELLITE;
          obj.par1 = i;
          obj.par2 = 0;

          return true;
        }
      }
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_ASTER, what))
  {
    // asteroids
    for (int i = 0; i < tAsteroids.count(); i++)
    {
      asteroid_t *a = &tAsteroids[i];

      if (!a->selected)
        continue;

      if (QString(a->name).contains(reg) || a->name == str)
      {        
        astSolve(a, mapView->jd);
        ra = a->orbit.lRD.Ra;
        dec = a->orbit.lRD.Dec;
        fov = AST_ZOOM;

        obj.type = MO_ASTER;
        obj.par1 = i;
        obj.par2 = (qint64)a;

        return(true);
      }
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_COMET, what))
  {
    // comets
    for (int i = 0; i < tComets.count(); i++)
    {
      comet_t *a = &tComets[i];

      if (!a->selected)
        continue;

      if (QString(a->name).contains(reg) || a->name == str)
      {
        comSolve(a, mapView->jd);
        ra = a->orbit.lRD.Ra;
        dec = a->orbit.lRD.Dec;

        if (g_skSet.map.comet.real)
        {
          fov = qMin(COM_ZOOM, 8 * D2R(a->orbit.params[2] / 3600.));
        }
        else
        {
          fov = COM_ZOOM;
        }

        obj.type = MO_COMET;
        obj.par1 = i;
        obj.par2 = (qint64)a;

        return(true);
      }
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_SHOWER, what))
  {
    const CMeteorShowerItem *item = g_meteorShower.search(str);

    if (item)
    {
      obj.type = MO_SHOWER;
      obj.par1 = (qint64)item;

      ra = item->rd.Ra;
      dec = item->rd.Dec;
      fov = D2R(45);
      precess(&ra, &dec, JD2000, mapView->jd);

      return true;
    }
  }

  QApplication::processEvents();

  if (SS_CHECK_OR(SS_LUNAR_FEAT, what))
  {
    if (cLunarFeatures.search(str, mapView, ra, dec, fov))
    {
      return(true);
    }
  }

  return(false);
}
