#include "csearch.h"
#include "casterdlg.h"
#include "ccomedit.h"
#include "clunarfeatures.h"
#include "csgp4.h"

//////////////////
CSearch::CSearch()
//////////////////
{
}

///////////////////////////////////////////////////////////////////////////////////////////
bool CSearch::search(mapView_t *mapView, QString str, double &ra, double &dec, double &fov)
///////////////////////////////////////////////////////////////////////////////////////////
{
  QRegExp reg("\\b" + str + "\\b", Qt::CaseInsensitive);

  str = str.simplified();

  // ra/dec
  {
    QStringList list = str.split(' ');
    if (list.count() == 6)
    {
      double rah, ram, ras;
      double decd, decm, decs;

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
        return true;
      }
    }
  }

  QApplication::processEvents();

  // constellation
  if (constFind(str, ra, dec, fov, mapView->jd))
    return(true);

  QApplication::processEvents();

  // star names
  for (int i = 0; i < cTYC.tNames.count(); i++)
  {
    int offs = cTYC.tNames[i]->supIndex;
    QString name = cTYC.getStarName(&cTYC.pSupplement[offs]);

    if (!str.compare(name, Qt::CaseInsensitive))
    {
      ra = cTYC.tNames[i]->rd.Ra;
      dec = cTYC.tNames[i]->rd.Dec;
      precess(&ra, &dec, JD2000, mapView->jd);
      fov = D2R(30);
      return(true);
    }
  }

  QApplication::processEvents();

  // search planet/sun/moon
  for (int i = PT_SUN; i <= PT_MOON; i++)
  {
    orbit_t o;
    cAstro.calcPlanet(i, &o);
    QString name = o.name;
    QString english = o.englishName;
    if (!str.compare(name, Qt::CaseInsensitive) ||
        !str.compare(english, Qt::CaseInsensitive))
    {
      ra = o.lRD.Ra;
      dec = o.lRD.Dec;
      fov = getOptObjFov(o.sx / 3600.0, o.sy / 3600.0);
      return(true);
    }
  }

  QApplication::processEvents();

  // satellites
  QString satName = str;

  for (int i = 0; i < sgp4.count(); i++)
  {
    satellite_t out;
    radec_t rd;

    if (sgp4.getName(i).compare(satName, Qt::CaseInsensitive) == 0)
    {
      if (sgp4.solve(i, mapView, &out))
      {
        cAstro.convAA2RDRef(out.azimuth, out.elevation, &rd.Ra, &rd.Dec);

        ra = rd.Ra;
        dec = rd.Dec;
        fov = getOptObjFov(0, 0, D2R(2.5));

        return true;
      }
    }
  }

  QApplication::processEvents();

  // asteroids
  for (int i = 0; i < tAsteroids.count(); i++)
  {
    asteroid_t *a = &tAsteroids[i];

    if (!a->selected)
      continue;

    if (QString(a->name).contains(reg))
    {
      astSolve(a, mapView->jd);
      ra = a->orbit.lRD.Ra;
      dec = a->orbit.lRD.Dec;
      fov = AST_ZOOM;
      return(true);
    }
  }

  QApplication::processEvents();

  // comets
  for (int i = 0; i < tComets.count(); i++)
  {
    comet_t *a = &tComets[i];

    if (!a->selected)
      continue;

    if (QString(a->name).contains(reg))
    {
      comSolve(a, mapView->jd);
      ra = a->orbit.lRD.Ra;
      dec = a->orbit.lRD.Dec;
      fov = COM_ZOOM;
      return(true);
    }
  }

  QApplication::processEvents();

  // lunar features : TODO: dodelat
  if (cLunarFeatures.search(str, mapView, ra, dec, fov))
  {
    return(true);
  }

  QApplication::processEvents();

  // dso
  dso_t *dso;
  if (cDSO.findDSO((char *)qPrintable(str), &dso) != -1)
  {
    ra = dso->rd.Ra;
    dec = dso->rd.Dec;
    precess(&ra, &dec, JD2000, mapView->jd);
    fov = getOptObjFov(dso->sx / 3600., dso->sy / 3600.);
    return(true);
  }

  QApplication::processEvents();

  return(false);
}
