#ifndef CSEARCH_H
#define CSEARCH_H

#include "skcore.h"
#include "cmapview.h"
#include "castro.h"
#include "transform.h"
#include "cdso.h"
#include "constellation.h"
#include "tycho.h"
#include "mapobj.h"

#define SS_ALL          "'**'"
#define SS_PLANET       "'PL'"
#define SS_STAR         "'ST'"
#define SS_STAR_NAME    "'SN'"
#define SS_POS          "'PS'"
#define SS_CONSTEL      "'CN'"
#define SS_DSO          "'DO'"
#define SS_ART_SAT      "'AS'"
#define SS_ASTER        "'AT'"
#define SS_COMET        "'CO'"
#define SS_LUNAR_FEAT   "'LF'"
#define SS_SHOWER       "'MS'"

#define SS_CHECK_OR(a, str)     (!str.compare(a) || !str.compare(SS_ALL))

class CSearch
{
public:
  CSearch();
  static bool search(mapView_t *mapView, QString str, double &ra, double &dec, double &fov, mapObj_t &obj);
};

#endif // CSEARCH_H
