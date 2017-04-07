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
#include "cucac4.h"
#include "csgp4.h"
#include "c3dsolar.h"
#include "cdrawing.h"
#include "cmeteorshower.h"
#include "gcvs.h"
#include "vocatalogrenderer.h"

extern MainWindow *pcMainWnd;
extern CMapView   *pcMapView;
static QList      <mapObj_t> tObj;

typedef struct
{
  qint64   objIdx;
  int      objType;  // MO_xxx
  QString  objName;
} holdObject_t;

bool          g_bHoldObject = false;
holdObject_t  g_HoldObject;

void holdObject(int type, int id, const QString &name)
{
  g_bHoldObject = true;
  g_HoldObject.objName = name;
  g_HoldObject.objIdx = id;
  g_HoldObject.objType = type;
}

////////////////////////////////
// -1 = remove any
void releaseHoldObject(int type)
////////////////////////////////
{
  if (!g_bHoldObject)
    return;

  if (g_HoldObject.objType == type || type == -1)
  {
    g_bHoldObject = false;
    pcMainWnd->enableReleaseObject(false);
  }
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
  if (g_HoldObject.objType == MO_EARTH_SHD)
  {
    orbit_t o, m;

    cAstro.setParam(&p->m_mapView);
    cAstro.calcPlanet(PT_MOON, &m);
    cAstro.calcEarthShadow(&o, &m);
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
  else
  if (g_HoldObject.objType == MO_SATELLITE)
  {
    radec_t rd;
    satellite_t s;

    sgp4.solve(g_HoldObject.objIdx, &p->m_mapView, &s);
    cAstro.setParam(&p->m_mapView);
    cAstro.convAA2RDRef(s.azimuth, s.elevation, &rd.Ra, &rd.Dec);

    p->centerMap(rd.Ra, rd.Dec, CM_UNDEF);
  }
  else
  if (g_HoldObject.objType == MO_PLN_SAT)
  {
    int pln = g_HoldObject.objIdx & 0xffff;
    int index = (g_HoldObject.objIdx & 0xffff0000) >> 16;
    CPlanetSatellite planSat;
    planetSatellites_t sats;

    orbit_t earth, o;

    cAstro.setParam(&p->m_mapView);
    cAstro.calcPlanet(PT_EARTH, &earth, false, true, false);
    cAstro.calcPlanet(pln, &o);

    planSat.solve(p->m_mapView.jd - o.light, pln, &sats, &o, &earth);

    double ra = sats.sats[index].lRD.Ra;
    double dec = sats.sats[index].lRD.Dec;

    precess(&ra, &dec, JD2000, p->m_mapView.jd);

    p->centerMap(ra, dec, CM_UNDEF);
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void addMapObj(const radec_t &rd, int x, int y, int type, int selType, int size, qint64 par1, qint64 par2, double mag)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  mapObj_t o;

  o.rd = rd; // J2000
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
  qint64 border = 5;

  switch (obj->selType)
  {
    case MO_CIRCLE:
      {
        qint64 deltaX = abs(pos.x() - obj->x);
        qint64 deltaY = abs(pos.y() - obj->y);
        qint64 dist = POW2(deltaX) + POW2(deltaY);

        if (dist <= POW2(obj->size + border))
          return(true);
      }
      break;

    case MO_RECT:
      {
        QRectF rc(obj->x - (obj->size + border), obj->y - (obj->size + border), (obj->size + border) * 2, (obj->size + border) * 2);

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
    if (o1.type == MO_VOCATALOG || o1.type == MO_DSO || o1.type == MO_PLANET) // sort by size
      return(o1.size < o2.size);
    else // else by magnitude
      return(o1.mag < o2.mag);
  }
  return(false);
}

////////////////////////////////////////////////////////
bool mapObjSnapAll(int x, int y, radec_t *rd, int &type)
////////////////////////////////////////////////////////
{
  for (int i = 0; i < tObj.count(); i++)
  {
    mapObj_t o = tObj[i];

    if (o.type != MO_INSERT && checkMapObjPos(QPoint(x, y), &tObj[i]))
    {      
      rd->Ra = o.rd.Ra;
      rd->Dec = o.rd.Dec;            
      type = o.type;

      return(true);
    }
  }

  return(false);
}

//////////////////////////////////////////////
bool mapObjSnap(int x, int y, radec_t *rd)
//////////////////////////////////////////////
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
  QList       <mapObj_t> tObjTmp;
  QAction     *a;
  QMenu        myMenu;
  QPoint       pos = QCursor::pos();
  QPoint       wpos = map->mapFromGlobal(pos);

  QList <QString>  strSuf;
  QList <int>      strIdx;
  QMap <QAction *, int> drawingMap;

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
    if (g_pTelePlugin->getAttributes() & TPI_CAN_SLEW)
    {
      myMenu.addSeparator();
      a = myMenu.addAction(QIcon(":res/ico_find_tele.png"), QObject::tr("Slew telescope"));
      a->setData(-3);
    }

    if (g_pTelePlugin->getAttributes() & TPI_CAN_SYNC)
    {
      ofiItem_t *info = pcMainWnd->getQuickInfo();
      if (info == NULL)
      {
        a = myMenu.addAction(QObject::tr("Sync telescope to cursor position"));
      }
      else
      {
        a = myMenu.addAction(QObject::tr("Sync telescope to ") + info->title);
      }
      a->setData(-7);
      myMenu.addSeparator();
    }
    else
    {
      myMenu.addSeparator();
    }
  }

  // TODO: udelat vice (hold objectu) najednou

  mapObj_t o;
  QString  str;
  int index = 0;
  int lastType = -1;
  bool isHoldObjFirst = false;
  int cometIndex = -1;
  int astIndex = -1;
  QString noradName;

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

      case MO_INSERT:
      {
        QString name;

        drawing_t *d = getDrawing(o.par1);

        switch (d->type)
        {
          case DT_TEXT:
            name = d->text_t.text;
            break;

          case DT_TELRAD:
            name = QObject::tr("Telrad");
            break;

          case DT_TELESCOPE:
            name = d->telescope_t.name;
            break;

          case DT_FRM_FIELD:
          case DT_EXT_FRAME:
            name = d->frmField_t.text;
            break;

          default:
            name = "???";
        }

        //if (!getDrawing(o.par1)->telescopeLink && !g_pTelePlugin)
        if (!g_pTelePlugin || ((!getDrawing(o.par1)->telescopeLink && g_pTelePlugin) || getDrawing(o.par1)->type == DT_FRM_FIELD))
        {
          a = myMenu.addAction(QObject::tr("Edit object '") + name + "'");
          drawingMap[a] = o.par1;
          a->setData(-10);
        }

        if (g_pTelePlugin && pcMapView->m_lastTeleRaDec.Ra != CM_UNDEF)
        {
          if (getDrawing(o.par1)->telescopeLink)
            a = myMenu.addAction(QObject::tr("Unlink object '%1' from telescope").arg(name));
          else
            a = myMenu.addAction(QObject::tr("Link object '%1' to telescope").arg(name));
          drawingMap[a] = o.par1;
          a->setData(-11);
        }
        continue;
      }

      case MO_EARTH_SHD:
      {
        str = QObject::tr("Earth shadow");

        if (!g_bHoldObject && !isHoldObjFirst)
        {
          strSuf.append(cHoldObj + str);
          strIdx.append(-4);

          g_HoldObject.objName = str;
          g_HoldObject.objType = MO_EARTH_SHD;
          isHoldObjFirst =  true;
        }

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
        break;
      }

      case MO_PLN_SAT:
      {
        CPlanetSatellite planSat;
        planetSatellites_t sats;
        orbit_t  pl;
        orbit_t  s;

        cAstro.calcPlanet(o.par1, &pl);
        cAstro.calcPlanet(PT_EARTH, &s, true, true, false);

        planSat.solve(map->m_mapView.jd - pl.light, o.par1, &sats, &pl, &s);

        str = sats.sats[o.par2].name + " " + getStrMag(o.mag);

        if (!g_bHoldObject && !isHoldObjFirst)
        {
          strSuf.append(cHoldObj + sats.sats[o.par2].name);
          strIdx.append(-4);

          g_HoldObject.objName = sats.sats[o.par2].name;
          g_HoldObject.objIdx = o.par1 | (o.par2 << 16);
          g_HoldObject.objType = MO_PLN_SAT;
          isHoldObjFirst =  true;
        }

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

        if (astIndex == -1)
        { // only first asteroid in menu
          strSuf.append(QObject::tr("  Show asteroid ") + a->name + QObject::tr(" in 3D"));
          strIdx.append(-9);
          astIndex = o.par1;
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

        if (cometIndex == -1)
        { // only first comet in menu
          strSuf.append(QObject::tr("  Show comet ") + a->name + QObject::tr(" in 3D"));
          strIdx.append(-8);
          cometIndex = o.par1;
        }

        break;
      }

      case MO_SATELLITE:
      {
        str = sgp4.getName(o.par1);

        if (!g_bHoldObject && !isHoldObjFirst)
        {
          strSuf.append(cHoldObj + str);
          strIdx.append(-4);

          g_HoldObject.objName = str;
          g_HoldObject.objIdx = o.par1;
          g_HoldObject.objType = MO_SATELLITE;
          isHoldObjFirst =  true;
        }

        if (noradName.isEmpty())
        {
          strSuf.append(QObject::tr("  Find %1 in NORAD database").arg(str));
          strIdx.append(-12);
          noradName = sgp4.getID(o.par1);
        }
      }
      break;

      case MO_SHOWER:
      {
        CMeteorShowerItem *item = (CMeteorShowerItem *)o.par1;
        str = item->name;
      }
      break;

      case MO_DSO:
      {
        dso_t *dso = (dso_t *)o.par1;

        QString name = cDSO.getCommonName(dso);

        if (name.length() == 0)
          name = cDSO.getName(dso);

        str = name + ((dso->mag != NO_DSO_MAG) ? QString(QObject::tr(", %1 mag.")).arg(dso->DSO_MAG, 0, 'f', 2) : QString(""));
        break;
      }

      case MO_VOCATALOG:
      {
        VOItem_t *obj = (VOItem_t*)o.par2;

        str = QString("%1").arg(QString(obj->name)) + ", " + getStrMag(obj->mag);
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

      case MO_UCAC4:
      {
        ucac4Star_t s;

        cUcac4.getStar(s, o.par1, o.par2);

        str = QString("UCAC4 %1-%2").arg(s.zone).arg(s.number, 6, 10, QChar('0')) + QString(QObject::tr(", %1 mag.")).arg(s.mag, 0, 'f', 2);
        break;
      }

      case MO_USNO2:
      {
        usnoStar_t s;
        usnoZone_t *z;

        z = usno.getStar(&s, o.par1, o.par2);

        str = QString("USNO A2 %1-%2").arg(z->zone).arg(s.id) + QString(QObject::tr(", %1 mag.")).arg(s.rMag, 0, 'f', 2);
        break;
      }

      case MO_USNOB1:
      {
        str = QString("USNO B1 %1-%2").arg(o.par1).arg(o.par2) + QString(QObject::tr(", %1 mag.")).arg(o.mag, 0, 'f', 2);
        break;
      }

      case MO_NOMAD:
      {
        str = QString("NOMAD %1-%2").arg(o.par1).arg(o.par2) + QString(QObject::tr(", %1 mag.")).arg(o.mag, 0, 'f', 2);
        break;
      }

      case MO_URAT1:
      {
        str = QString("URAT1 %1-%2").arg(o.par1).arg(o.par2) + QString(QObject::tr(", %1 mag.")).arg(o.mag, 0, 'f', 2);
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

        gcvs_t *gcvs = g_GCVS.getStar(t->tyc1, t->tyc2, t->tyc3);

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
              else if (gcvs)
              {
                str = gcvs->name;
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
          if (gcvs == nullptr)
          {
            str = QString("TYC %1-%2-%3").arg(t->tyc1).arg(t->tyc2).arg(t->tyc3);
          }
          else
          {
            str = gcvs->name;
          }
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
        pcMainWnd->enableReleaseObject(true);
        return;
      }

      case -5:
      {
        g_bHoldObject = false;
        pcMainWnd->enableReleaseObject(false);
        return;
      }

      case -7:
      {
        if (msgBoxQuest(pcMainWnd, QObject::tr("Do you really want to synchronize the telescope?")) == QMessageBox::Yes)
        {
          ofiItem_t *info = pcMainWnd->getQuickInfo();
          double r, d;
          if (info == NULL)
          {
            r = R2D(ra) / 15.0;
            d = R2D(dec);
          }
          else
          {
            double ra = info->radec.Ra, dec = info->radec.Dec;

            precess(&ra, &dec, JD2000, map->m_mapView.jd);

            r = R2D(ra) / 15.0;
            d = R2D(dec);
          }

          g_pTelePlugin->syncTo(r, d);
        }
        return;
      }

      case -8:
      {
        C3DSolar dlg(&pcMapView->m_mapView, pcMainWnd, true, cometIndex);

        if (dlg.exec() == DL_OK)
        {
          pcMapView->m_mapView.jd = dlg.jd();
        }

        pcMainWnd->repaintMap();
        return;
      }

      case -9:
      {
        C3DSolar dlg(&pcMapView->m_mapView, pcMainWnd, false, astIndex);

        if (dlg.exec() == DL_OK)
        {
          pcMapView->m_mapView.jd = dlg.jd();
        }

        pcMainWnd->repaintMap();
        return;
      }

      case -10:
      {
        g_cDrawing.selectAndEdit(drawingMap[selectedItem]);
        return;
      }

      case -11:
      {
        g_cDrawing.toggleTelescopeLink(getDrawing(drawingMap[selectedItem]));
        return;
      }

      case -12:
      {
        QString url;

        url = QString("https://www.n2yo.com/satellite/?s=%1").arg(noradName);

        QDesktopServices::openUrl(url);
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

QString checkObjOnMap(const QPoint &pos)
{
  mapObj_t obj;
  QString nameStr;
  QString magStr;

  if (mapObjSearch(pos.x(), pos.y(), &obj))
  {
    switch (obj.type)
    {
      case MO_TYCSTAR:
      {
        tychoStar_t *t;

        cTYC.getStar(&t, obj.par1, obj.par2);

        nameStr = QString("TYC %1-%2-%3").arg(t->tyc1).arg(t->tyc2).arg(t->tyc3);
        magStr = getStrMag(cTYC.getVisMag(t));
      }
      break;      

      case MO_GSCSTAR:
      {
        gsc_t g;

        cGSC.getStar(&g, obj.par1, obj.par2);
        nameStr = nameStr.sprintf("GSC %d-%d", g.reg, g.id);
        magStr = getStrMag(g.pMag);
        break;
      }

      case MO_USNO2:
      {
        usnoStar_t s;
        usnoZone_t *z;

        z = usno.getStar(&s, obj.par1, obj.par2);

        nameStr = QString("USNO A2 %1-%2").arg(z->zone).arg(s.id);
        magStr = getStrMag(s.rMag);
        break;
      }

      case MO_USNOB1:
      {
        nameStr = QString("USNO B1 %1-%2").arg(obj.par1).arg(obj.par2);
        magStr = getStrMag(obj.mag);
        break;
      }

      case MO_NOMAD:
      {
        nameStr = QString("NOMAD B1 %1-%2").arg(obj.par1).arg(obj.par2);
        magStr = getStrMag(obj.mag);
        break;
      }

      case MO_URAT1:
      {
        nameStr = QString("URAT1 %1-%2").arg(obj.par1).arg(obj.par2);
        magStr = getStrMag(obj.mag);
        break;
      }

      case MO_UCAC4:
      {
        ucac4Star_t s;

        cUcac4.getStar(s, obj.par1, obj.par2);

        nameStr = QString("UCAC4 %1-%2").arg(s.zone).arg(s.number, 6, 10, QChar('0'));
        magStr = getStrMag(s.mag);
        break;
      }

      case MO_PPMXLSTAR:
      {
        ppmxlCache_t *data;

        data = cPPMXL.getRegion(obj.par1);
        ppmxl_t *star = &data->data[obj.par2];

        nameStr = QString("PPMXL %1").arg(star->id);
        magStr = getStrMag(star->mag / 1000.0);
        break;
      }

      case MO_PLANET:
      {
        nameStr = cAstro.getName(obj.par1);
        magStr = getStrMag(obj.mag);
        break;
      }

      case MO_ASTER:
      {
        asteroid_t *a = (asteroid_t *)obj.par2;

        nameStr = QString(a->name);
        magStr = getStrMag(a->orbit.mag);
        break;
      }

      case MO_COMET:
      {
        comet_t *c = (comet_t *)obj.par2;

        nameStr = QString(c->name);
        magStr = getStrMag(c->orbit.mag);
        break;
      }

      case MO_SATELLITE:
      {
        nameStr = sgp4.getName(obj.par1);
        magStr = "";
      }
      break;

      case MO_DSO:
      {
        dso_t *dso = (dso_t *)obj.par1;

        nameStr = cDSO.getName(dso);
        magStr = ((dso->mag != NO_DSO_MAG) ? (getStrMag(dso->DSO_MAG)) : QString(""));
      }
      break;

      case MO_VOCATALOG:
      {
        VOItem_t *object = (VOItem_t*)obj.par2;

        nameStr = object->name;
        magStr = getStrMag(object->mag);
        break;
      }
    }

    return (nameStr + "\n" + magStr).trimmed();
  }

  return QString("");
}
