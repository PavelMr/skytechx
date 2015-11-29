#ifndef CRTS_H
#define CRTS_H

#include "skcore.h"
#include "cmapview.h"
#include "castro.h"

#define RTS_ERR      3 // RTS cannot be solved
#define RTS_CIRC     2 // circumpolar
#define RTS_NONV     1 // not visible
#define RTS_DONE     0 // ok

#define RTS_T_RISE      1
#define RTS_T_TRANSIT   2
#define RTS_T_SET       4

#define ALT_ERR        -1234
#define ALT_DIR_RISE       0
#define ALT_DIR_SET        1


typedef struct
{
  double rise;
  double transit;
  double set;
  double rAzm, sAzm, tAlt;
     int flag; // RTS_xxx
     int rts;  // RTS_T_xxx or
} rts_t;

typedef struct
{
  double beginAstroTw;
  double beginNauticalTw;
  double beginCivilTw;
  double endCivilTw;
  double endNauticalTw;
  double endAstroTw;
} daylight_t;

class CRts
{
public:
    CRts();

    void setLowPrec(void);
    void calcFixed(rts_t *rts, double ra, double dec, const mapView_t *view);
    void calcOrbitRTS(rts_t *rts, qint64 ptr, int type, const mapView_t *view, bool calcTransit = true);
    void calcTwilight(daylight_t *rts, mapView_t *view);

protected:
    bool calcSunPosAtAlt(double start, double atAlt, double *jdTo, mapView_t *view, bool center);
    bool isNotRTS(double dec, rts_t *rts, const mapView_t *view);
  double getRTSRaDecFromPtr(radec_t *rd, qint64 ptr, int type, double jd);

  CAstro *ast;
  bool   m_bLow;
  double m_limit;
};

#endif // CRTS_H
