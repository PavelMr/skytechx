#include "crts.h"
#include "cmapview.h"
#include "jd.h"
#include "mapobj.h"
#include "casterdlg.h"
#include "ccomdlg.h"

// TODO: kontrola jizni polokoule
// TODO: u fixed objektu pouzit alg. meeus str. 97

#define RTS_IADD        JD1SEC * 60 * 60 * 3   // 1h
#define MAX_RTS_ITIN    3000

CRts::CRts()
{
  m_bLow = false;
  m_limit = JD1SEC;
  ast = &cAstro;
}

///////////////////////
void CRts::setLowPrec()
///////////////////////
{
  m_bLow = true;
  m_limit = JD1SEC * 30;
  ast = &cAstro;
}


// return size of object /////////////////////////////////////////////////
double CRts::getRTSRaDecFromPtr(radec_t *rd, qint64 ptr, int type, double jd)
//////////////////////////////////////////////////////////////////////////
{
  switch (type)
  {
    case MO_PLANET :
    { // ptr = planet id
      orbit_t o;
      ast->calcPlanet(ptr, &o);
      rd->Ra = o.lRD.Ra;
      rd->Dec = o.lRD.Dec;
      if (ptr == PT_SUN || ptr == PT_MOON)
        return(DMS2RAD(0, 0, o.sy / 2));
      return(0);
    }
    break;

    case MO_ASTER :
    {// ptr = *asteroid_t
      asteroid_t c,*o = (asteroid_t *)ptr;
      c = *o;
      astSolve(&c, jd);
      rd->Ra = c.orbit.lRD.Ra;
      rd->Dec = c.orbit.lRD.Dec;
      return(0);
    }
    break;

    case MO_COMET :
    {// ptr = *comet_t
     comet_t c,*o = (comet_t *)ptr;
     c = *o;
     comSolve(&c, jd);
     rd->Ra = c.orbit.lRD.Ra;
     rd->Dec = c.orbit.lRD.Dec;
     return(0);
    }
    break;
  }

  //pcDebug->writeError("Unable to solve RTS (%d)\n", type);
  return(0);
}

/////////////////////////////////////////////////////////////////////////////
void CRts::calcOrbitRTS(rts_t *rts, qint64 ptr, int type, const mapView_t *view)
/////////////////////////////////////////////////////////////////////////////
{
  mapView_t v;
  double  tmpJD = view->jd;
  double  jd;
  double  add = RTS_IADD;
  double  curDay = (floor(view->jd + 0.5) - 0.5) - view->geo.tz;
  double  startDay = curDay;
  double  lAzm;
  double  lAlt;
  double  azm;
  double  alt;
  double  r;
  radec_t rd;

  jd = curDay;
  v = *view;

  memset(rts, 0, sizeof(rts_t));
  rts->flag = RTS_DONE;
  rts->rts = 0;

  int cnt = 0;

  // transit
  v.jd = jd;
  ast->setParam(&v);
  r = getRTSRaDecFromPtr(&rd, ptr, type, jd);
  ast->convRD2AARef(rd.Ra, rd.Dec, &lAzm, &lAlt);

  while (1)
  {
    v.jd = jd;
    ast->setParam(&v);
    r = getRTSRaDecFromPtr(&rd, ptr, type, jd);
    ast->convRD2AARef(rd.Ra, rd.Dec, &azm, &alt);

    if (azm >= R180 && lAzm <= R180)
    {
      jd -= add;
      add /= 2.0;
      if (add < m_limit) break;
    }
    else
    {
      jd += add;
    }
    if (++cnt > MAX_RTS_ITIN)
    {
      rts->flag = RTS_ERR;
      v.jd = tmpJD;
      return;
    }
    lAzm = azm;
  }

  rts->tAlt = alt;// + (alt - lAlt) / 2.0;
  rts->transit = jd;
  rts->rts |= RTS_T_TRANSIT;


  if (isNotRTS(rd.Dec, rts, view))
  { // no rise / set
    v.jd = tmpJD;
    return;
  }

  // rise
  jd = curDay;
  add = RTS_IADD;
  v.jd = jd;
  ast->setParam(&v);
  r = getRTSRaDecFromPtr(&rd, ptr, type, jd);
  ast->convRD2AARef(rd.Ra, rd.Dec, &lAzm, &lAlt, r);
  //lAlt += r;

  cnt = 0;
  while (1)
  {
    v.jd = jd;
    ast->setParam(&v);
    r = getRTSRaDecFromPtr(&rd, ptr, type, jd);
    ast->convRD2AARef(rd.Ra, rd.Dec, &azm, &alt, r);

    if (alt > 0 && lAlt < 0)
   {
     jd -= add;
     add /= 2.0;
     if (add < m_limit) break;
   }
   else
   {
     jd += add;
   }
   if (++cnt > MAX_RTS_ITIN)
   {
     rts->flag = RTS_ERR;
     v.jd = tmpJD;
     return;
   }
    lAlt = alt;
  }

  rts->rAzm = azm;
  rts->rise = jd;
  if (jd <= startDay + 1)
    rts->rts |= RTS_T_RISE;

  // set
  jd = curDay;
  add = RTS_IADD;
  v.jd = jd;
  ast->setParam(&v);
  r = getRTSRaDecFromPtr(&rd, ptr, type, jd);
  ast->convRD2AARef(rd.Ra, rd.Dec, &lAzm, &lAlt, r);

  cnt = 0;
  while (1)
  {
    v.jd = jd;
    ast->setParam(&v);
    r = getRTSRaDecFromPtr(&rd, ptr, type, jd);
    ast->convRD2AARef(rd.Ra, rd.Dec, &azm, &alt, r);

    if (alt < 0 && lAlt > 0)
   {
     jd -= add;
     add /= 2.0;
     if (add < m_limit) break;
   }
   else
   {
     jd += add;
   }
   if (++cnt > MAX_RTS_ITIN)
   {
     rts->flag = RTS_ERR;
     v.jd = tmpJD;
     return;
   }
    lAlt = alt;
  }

  rts->sAzm = azm;
  rts->set = jd;

  if (jd <= startDay + 1)
    rts->rts |= RTS_T_SET;
}


//////////////////////////////////////////////////////////////////////////////
// star/dso
void CRts::calcFixed(rts_t *rts, double ra, double dec, const mapView_t *view)
//////////////////////////////////////////////////////////////////////////////
{
  mapView_t v;
  double jd;
  double add = RTS_IADD;
  double curDay = (floor(view->jd + 0.5) - 0.5) - view->geo.tz;
  double startDay = curDay;
  double lAzm;
  double lAlt;
  double azm;
  double alt;

  jd = curDay;

  v = *view;

  memset(rts, 0, sizeof(rts_t));
  rts->flag = RTS_DONE;
  rts->rts = 0;

  // transit
  v.jd = jd;
  ast->setParam(&v);
  ast->convRD2AARef(ra, dec, &lAzm, &lAlt);

  int cnt = 0;
  while (1)
  {
    v.jd = jd;
    ast->setParam(&v);
    ast->convRD2AARef(ra, dec, &azm, &alt);

    if (azm >= R180 && lAzm <= R180)
    {
      jd -= add;
      add *= 0.5;
      if (add < m_limit) break;
    }
    else
    {
      jd += add;
    }
    if (++cnt > MAX_RTS_ITIN)
    {
      rts->flag = RTS_ERR;
      return;
    }
    lAzm = azm;
  }

  rts->tAlt = alt;
  rts->transit = jd;

  if (isNotRTS(dec, rts, view))
  { // no rise / set
    return;
  }

  rts->rts |= RTS_T_TRANSIT;

  // rise
  jd = curDay;
  add = RTS_IADD;
  v.jd = jd;
  ast->setParam(&v);
  ast->convRD2AARef(ra, dec, &lAzm, &lAlt);

  cnt = 0;
  while (1)
  {
    v.jd = jd;
    ast->setParam(&v);
    ast->convRD2AARef(ra, dec, &azm, &alt);

    if (alt > 0 && lAlt < 0)
   {
     jd -= add;
     add /= 2.0;
     if (add < m_limit) break;
   }
   else
   {
     jd += add;
   }
   if (++cnt > MAX_RTS_ITIN)
   {
     rts->flag = RTS_ERR;
     return;
   }
    lAlt = alt;
  }

  rts->rAzm = azm;
  rts->rise = jd;

  if (jd <= startDay + 1)
    rts->rts |= RTS_T_RISE;


  // set
  jd = curDay;
  add = RTS_IADD;
  v.jd = jd;
  ast->setParam(&v);
  ast->convRD2AARef(ra, dec, &lAzm, &lAlt);

  cnt = 0;
  while (1)
  {
    v.jd = jd;
    ast->setParam(&v);
    ast->convRD2AARef(ra, dec, &azm, &alt);

    if (alt < 0 && lAlt > 0)
   {
     jd -= add;
     add /= 2.0;
     if (add < m_limit) break;
   }
   else
   {
     jd += add;
   }
   if (++cnt > MAX_RTS_ITIN)
   {
     rts->flag = RTS_ERR;
     return;
   }
    lAlt = alt;
  }

  rts->sAzm = azm;
  rts->set = jd;

  if (jd <= startDay + 1)
    rts->rts |= RTS_T_SET;
}


/////////////////////////////////////////////////////////
void CRts::calcTwilight(daylight_t *rts, mapView_t *view)
/////////////////////////////////////////////////////////
{
  double    startDay = (floor(view->jd + 0.5) - 0.5) - view->geo.tz;
  mapView_t v;
  double    jd;

  v = *view;

  memset(rts, 0, sizeof(daylight_t));

  if (calcSunPosAtAlt(startDay, DEG2RAD(-18), &jd, &v, true))
  {
    rts->beginAstroTw = jd;
  }

  if (calcSunPosAtAlt(startDay, DEG2RAD(-12), &jd, &v, true))
  {
    rts->beginNauticalTw = jd;
  }

  if (calcSunPosAtAlt(startDay, DEG2RAD(-6), &jd, &v, true))
  {
    rts->beginCivilTw = jd;
  }

  startDay += 0.5;

  if (calcSunPosAtAlt(startDay, DEG2RAD(-18), &jd, &v, true))
  {
    rts->endAstroTw = jd;
  }

  if (calcSunPosAtAlt(startDay, DEG2RAD(-12), &jd, &v, true))
  {
    rts->endNauticalTw = jd;
  }

  if (calcSunPosAtAlt(startDay, DEG2RAD(-6), &jd, &v, true))
  {
    rts->endCivilTw = jd;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////
bool CRts::calcSunPosAtAlt(double start, double atAlt, double *jdTo, mapView_t *view, bool center)
//////////////////////////////////////////////////////////////////////////////////////////////////
{
  double  jd = start;
  double  add = RTS_IADD;
  int     cnt;
  double  lAzm, lAlt;
  double  alt, azm;
  radec_t rd;

  view->jd = jd;
  ast->setParam(view);
  double r;
  r = getRTSRaDecFromPtr(&rd, PT_SUN, MO_PLANET, jd);
  if (center) r = 0;
  ast->convRD2AARef(rd.Ra, rd.Dec, &lAzm, &lAlt, r);

  cnt = 0;
  while (1)
  {
    view->jd = jd;
    ast->setParam(view);
    r = getRTSRaDecFromPtr(&rd, PT_SUN, MO_PLANET, jd);
    if (center) r = 0;
    ast->convRD2AARef(rd.Ra, rd.Dec, &azm, &alt, r);
    if ((alt > atAlt && lAlt < atAlt) || (alt < atAlt && lAlt > atAlt))
    {
      jd -= add;
      add /= 2.0;
      if (add < m_limit) break;
    }
    else
    {
      jd += add;
    }
    if (++cnt > MAX_RTS_ITIN)
    {
      return(false);
    }
    lAlt = alt;
  }

  if (jd > start + 1)
    return(false);

  *jdTo = jd;

  return(true);
}


///////////////////////////////////////////////////////////////////
bool CRts::isNotRTS(double dec, rts_t *rts, const  mapView_t *view)
///////////////////////////////////////////////////////////////////
{
  double i;

  if (view->geo.lat >= 0)
    i = (R90 - view->geo.lat) + dec;
  else
    i = view->geo.lat - dec + R90;

  double eqt = 0;//ast->getInvAtmRef(0);

  if (i < eqt)// TODO: zkontrolovat
  {
    rts->flag = RTS_NONV;
    return(true);
  }

  if (view->geo.lat >= eqt)
  {
    i = R90 - view->geo.lat;
    if (dec >= i)
    {
      rts->flag = RTS_CIRC;
      return(true);
    }
  } else
  {
    i = -R90 - view->geo.lat;
    if (dec < i)
    {
      rts->flag = RTS_CIRC;
      return(true);
    }
  }

  return(false);
}

