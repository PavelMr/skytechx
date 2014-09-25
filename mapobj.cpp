#include "mapobj.h"
#include "cobjinfo.h"
#include "mainwindow.h"
#include "cdssdlg.h"
#include "cteleplug.h"
#include "csohoimg.h"
#include "casterdlg.h"
#include "ccomdlg.h"
#include "csatxyz.h"
#include "Usno2A.h"

extern MainWindow *pcMainWnd;
extern CMapView   *pcMapView;
static QList <mapObj_t> tObj;

typedef struct
{
  int      objIdx;
  int      objType;  // MO_xxx
  QString  objName;
} holdObject_t;

bool          g_bHoldObject = false;
holdObject_t  g_HoldObject;


////////////////////////////////
// -1 = remove any
void releaseHoldObject(int type)
////////////////////////////////
{
  if (!g_bHoldObject)
    return;

  if (g_HoldObject.objType == type || type == -1)
    g_bHoldObject = false;
}

///////////////////////////////////////////////////
void recenterHoldObject(CMapView *p, bool bRepaint)
///////////////////////////////////////////////////
{
  pcMainWnd->centerSearchBox(false);

  if (!g_bHoldObject)
  {
    return;
  }

  if (g_HoldObject.objType == MO_PLANET)
  {
    orbit_t o;

    cAstro.setParam(&p->m_mapView);
    cAstro.calcPlanet(g_HoldObject.objIdx, &o);
    p->centerMap(o.lRD.Ra, o.lRD.Dec, CM_UNDEF);
  }
  else
  if (g_HoldObject.objType == MO_TELESCOPE)
  {
    p->centerMap(pcMapView->m_lastTeleRaDec.Ra, pcMapView->m_lastTeleRaDec.Dec, CM_UNDEF);
  }
  else
  if (g_HoldObject.objType == MO_ASTER)
  {
    asteroid_t *a = (asteroid_t *)g_HoldObject.objIdx;

    cAstro.setParam(&p->m_mapView);
    astSolve(a, pcMapView->m_mapView.jd);
    p->centerMap(a->orbit.lRD.Ra, a->orbit.lRD.Dec, CM_UNDEF);
  }
  else
  if (g_HoldObject.objType == MO_COMET)
  {
    comet_t *a = (comet_t *)g_HoldObject.objIdx;

    cAstro.setParam(&p->m_mapView);
    comSolve(a, pcMapView->m_mapView.jd);
    p->centerMap(a->orbit.lRD.Ra, a->orbit.lRD.Dec, CM_UNDEF);
  }

  if (bRepaint)
    p->repaintMap();
}


//////////////////////
void mapObjReset(void)
//////////////////////
{
  tObj.clear();
}


/////////////////////////////////////////////////////////////////////////////////////////////
void addMapObj(int x, int y, int type, int selType, int size, int par1, int par2, double mag)
/////////////////////////////////////////////////////////////////////////////////////////////
{
  mapObj_t o;

  o.x = x;
  o.y = y;
  o.type = type;
  o.selType = selType;
  o.size = size;
  o.par1 = par1;
  o.par2 = par2;
  o.mag = mag;

  tObj.append(o);
}

/////////////////////////////////////////////////////
static bool checkMapObjPos(QPoint pos, mapObj_t *obj)
/////////////////////////////////////////////////////
{
  int border = 5;

  switch (obj->selType)
  {
    case MO_CIRCLE:
      {
        int deltaX = abs(pos.x() - obj->x);
        int deltaY = abs(pos.y() - obj->y);
        int dist = POW2(deltaX) + POW2(deltaY);

        if (dist <= POW2(obj->size + border))
          return(true);
      }
      break;

    case MO_RECT:
      {
        QRect rc(obj->x - (obj->size + border), obj->y - (obj->size + border), (obj->size + border) * 2, (obj->size + border) * 2);

        if (rc.contains(pos))
          return(true);
      }
      break;
  }
  return(false);
}


///////////////////////////////////////////////
static bool sortObj(mapObj_t &o1, mapObj_t &o2)
///////////////////////////////////////////////
{
  if (o1.type < o2.type)
    return(true);

  if (o1.type == o2.type)
  {
    if (o1.type == MO_DSO) // sort dso by size
      return(o1.size < o2.size);
    else // else by magnitude
      return(o1.mag < o2.mag);
  }
  return(false);
}


//////////////////////////////////////////
bool mapObjSnap(int x, int y, radec_t *rd)
//////////////////////////////////////////
{
  for (int i = 0; i < tObj.count(); i++)
  {
    if (tObj[i].type == MO_TYCSTAR)
    {
      if (checkMapObjPos(QPoint(x, y), &tObj[i]))
      {
        mapObj_t     o = tObj[i];
        tychoStar_t *t;

        cTYC.getStar(&t, o.par1, o.par2);

        rd->Ra = t->rd.Ra;
        rd->Dec = t->rd.Dec;

        return(true);
      }
    }
  }

  return(false);
}

//////////////////////////////////////////////
bool mapObjSearch(int x, int y, mapObj_t *obj)
//////////////////////////////////////////////
{
  mapObj_t o;
  QPoint   wpos = QPoint(x, y);

  qSort(tObj.begin(), tObj.end(), sortObj);

  for (int i = 0; i < tObj.count(); i++)
  {
    o = tObj[i];
    if (checkMapObjPos(wpos, &o))
    {
      *obj = o;
      return(true);
    }
  }

  return(false);
}


/////////////////////////////////////
void mapObjContextMenu(CMapView *map)
/////////////////////////////////////
{
  QList <mapObj_t> tObjTmp;
  QAction     *a;
  QMenu        myMenu;
  QPoint       pos = QCursor::pos();
  QPoint       wpos = map->mapFromGlobal(pos);

  QList <QString>  strSuf;
  QList <int>      strIdx;

  QString cHoldObj = QObject::tr("  Hold object ");

  tObjTmp = tObj;

  qSort(tObjTmp.begin(), tObjTmp.end(), sortObj);

  a = myMenu.addAction(QIcon(":res/ico_center.png"), QObject::tr("Center map"));
  a->setData(-1);

  if (map->m_mapView.fov <= D2R(45))
  {
    myMenu.addSeparator();
    a = myMenu.addAction(QIcon(":res/ico_dss_down.png"), QObject::tr("Download DSS"));
    a->setData(-2);
  }

  if (g_bHoldObject)
  {
    myMenu.addSeparator();
    a = myMenu.addAction(QObject::tr("Release object ") + g_HoldObject.objName);
    a->setData(-5);
  }

  if (g_pTelePlugin)
  {
    myMenu.addSeparator();
    a = myMenu.addAction(QIcon(":res/ico_find_tele.png"), QObject::tr("Slew telescope"));
    a->setData(-3);
    a = myMenu.addAction(QObject::tr("Sync telescope"));
    a->setData(-7);
    myMenu.addSeparator();
  }

  // TODO: udelat vice (hold objectu) najednou

  mapObj_t o;
  QString  str;
  int index = 0;
  int lastType = -1;
  bool isHoldObjFirst = false;

  foreach(o, tObjTmp)
  {
    strSuf.clear();
    strIdx.clear();

    if (!checkMapObjPos(wpos, &o))
    {
      index++;
      continue;
    }

    if (lastType != o.type)
    {
      lastType = o.type;
      myMenu.addSeparator();
    }

    switch (o.type)
    {
      case MO_TELESCOPE:
      {
        if (!g_bHoldObject && !isHoldObjFirst)
        {
          a = myMenu.addAction(QObject::tr("Hold ") + g_pTelePlugin->getTelescope());
          a->setData(-4);

          g_HoldObject.objName = g_pTelePlugin->getTelescope();
          g_HoldObject.objIdx = 0;
          g_HoldObject.objType = MO_TELESCOPE;
          isHoldObjFirst =  true;
        }
        continue;
      }

      case MO_EARTH_SHD:
      {
        str = QObject::tr("Earth shadow");
        break;
      }

      case MO_PLANET:
      {
        str = cAstro.getName(o.par1) + " " + getStrMag(o.mag);

        if (!g_bHoldObject && !isHoldObjFirst)
        {
          strSuf.append(cHoldObj + cAstro.getName(o.par1));
          strIdx.append(-4);

          g_HoldObject.objName = cAstro.getName(o.par1);
          g_HoldObject.objIdx = o.par1;
          g_HoldObject.objType = MO_PLANET;
          isHoldObjFirst =  true;
        }

        if (o.par1 == PT_SUN)
        {
          strSuf.append(QObject::tr("  Set Sun texture"));
          strIdx.append(-6);
        }
        break;
      }

      case MO_PLN_SAT:
      {
        satxyz_t sat;
        orbit_t  pl;
        orbit_t  s;

        cAstro.calcPlanet(o.par1, &pl);
        cAstro.calcPlanet(PT_SUN, &s);
        cSatXYZ.solve(map->m_mapView.jd, o.par1, &pl, &s, &sat);

        str = sat.sat[o.par2].name + " " + getStrMag(sat.sat[o.par2].mag);

        break;
      }

      case MO_ASTER:
      {
        asteroid_t *a = (asteroid_t *)o.par2;

        str = QString(a->name) + " " + getStrMag(a->orbit.mag);

        if (!g_bHoldObject && !isHoldObjFirst)
        {
          strSuf.append(cHoldObj + a->name);
          strIdx.append(-4);

          g_HoldObject.objName = a->name;
          g_HoldObject.objIdx = o.par2;
          g_HoldObject.objType = MO_ASTER;
          isHoldObjFirst =  true;
        }
        break;
      }

    case MO_COMET:
    {
      comet_t *a = (comet_t *)o.par2;

      str = QString(a->name) + " " + getStrMag(a->orbit.mag);

      if (!g_bHoldObject && !isHoldObjFirst)
      {
        strSuf.append(cHoldObj + a->name);
        strIdx.append(-4);

        g_HoldObject.objName = a->name;
        g_HoldObject.objIdx = o.par2;
        g_HoldObject.objType = MO_COMET;
        isHoldObjFirst =  true;
      }
      break;
    }

      case MO_DSO:
      {
        dso_t *dso = (dso_t *)o.par1;

        QString name = cDSO.getCommonName(dso);

        if (name.length() == 0)
          name = cDSO.getName(dso);

        str = name + ((dso->mag != NO_DSO_MAG) ? QString(QObject::tr(", %1 mag.")).arg(dso->DSO_MAG, 0, 'f', 2) : QString(""));
        break;
      }

      case MO_PPMXLSTAR:
      {
        ppmxlCache_t *data;

        data = cPPMXL.getRegion(o.par1);
        ppmxl_t *star = &data->data[o.par2];

        str = QString("PPMXL %1").arg(star->id) + QString(QObject::tr(", %1 mag.")).arg(star->mag / 1000.0, 0, 'f', 2);
        break;
      }

      case MO_USNOSTAR:
      {
        usnoStar_t s;
        usnoZone_t *z;

        z = usno.getStar(&s, o.par1, o.par2);

        str = QString("USNO2 %1-%2").arg(z->zone).arg(s.id) + QString(QObject::tr(", %1 mag.")).arg(s.rMag, 0, 'f', 2);
        break;
      }

      case MO_GSCSTAR:
      {
        gsc_t g;

        cGSC.getStar(&g, o.par1, o.par2);
        str = str.sprintf("GSC %d-%d", g.reg, g.id) + QString(QObject::tr(", %1 mag.")).arg(g.pMag, 0, 'f', 2);
        break;
      }

      case MO_TYCSTAR:
      {
        tychoStar_t *t;
        QString      tmp;

        cTYC.getStar(&t, o.par1, o.par2);

        QString mag = getStrMag(cTYC.getVisMag(t));

        if (t->supIndex != -1)
        {
          bool bayer, flam;
          tychoSupp_t *supp = &cTYC.pSupplement[t->supIndex];

          int con = constWhatConstel(t->rd.Ra, t->rd.Dec, JD2000);

          tmp = cTYC.getStarName(supp);
          if (tmp.length() > 0)
          {
            str = tmp;
          }
          else
          {
            tmp = cTYC.getBayerFullStr(supp, bayer);
            if (tmp.length() > 0)
            {
              str = tmp + " " + constGetName(con, 2);
            }
            else
            {
              tmp = cTYC.getFlamsteedStr(supp, flam);
              if (tmp.length() > 0)
              {
                str = tmp + " " + constGetName(con, 2);
              }
              else
              {
                str = QString("HD %1").arg(supp->hd);
              }
            }
          }
        }
        else
        {
          str = QString("TYC %1-%2-%3").arg(t->tyc1).arg(t->tyc2).arg(t->tyc3);
        }
        str += " " + mag;
        break;
      }
      default:
        str = "???";
    }

    a = myMenu.addAction(str);
    a->setData(index);
    index++;

    for (int i = 0; i < strSuf.count(); i++)
    {
      a = myMenu.addAction(strSuf[i]);
      a->setData(strIdx[i]);
    }
  }

  QAction *selectedItem = myMenu.exec(pos);
  double   ra, dec;

  trfConvScrPtToXY(wpos.x(), wpos.y(), ra, dec);

  if (selectedItem)
  {
    int idx = selectedItem->data().toInt();
    switch (idx)
    {
      case -1:
        {
          map->centerMap(ra, dec, CM_UNDEF);
          return;
        }

      case -2:
        {
          CDSSDlg dlg(pcMainWnd, ra, dec, map->m_mapView.jd);
          dlg.exec();
          return;
        }

      case -3:
      {
        double r = R2D(ra) / 15.0;
        double d = R2D(dec);
        g_pTelePlugin->slewTo(r, d);
        return;
      }

      case -4:
      {
        g_bHoldObject = true;
        return;
      }

      case -5:
      {
        g_bHoldObject = false;
        return;
      }

      case -6:
      {
        CSohoImg dlg(map);

        dlg.exec();
        return;
      }

      case -7:
      {
        if (msgBoxQuest(pcMainWnd, QObject::tr("Do you really want to synchronize the telescope?")) == QMessageBox::Yes)
        {
          double r = R2D(ra) / 15.0;
          double d = R2D(dec);
          g_pTelePlugin->syncTo(r, d);
        }
        return;
      }

      default:
        CObjInfo dlg(map);

        dlg.init(map, &map->m_mapView, &tObjTmp[idx]);
        dlg.exec();
        pcMainWnd->fillQuickInfo(&dlg.m_infoItem);
    }
  }
}

