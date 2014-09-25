#ifndef PLANE_H
#define PLANE_H

#include "vecmath.h"

typedef struct
{
  double x, y, z;
  double dist;
} SKPLANE;

SKPLANE *SKPLANEFromPoint(SKPLANE *out, SKVECTOR *a, SKVECTOR *b, SKVECTOR *c);

bool SKPLANECheckFrustumToPoint(SKPLANE *frustum, SKVECTOR *point);
bool SKPLANECheckFrustumToSphere(SKPLANE *frustum, SKVECTOR *point, double radius);
bool SKPLANECheckFrustumToLine(SKPLANE *frustum, SKVECTOR *p1, SKVECTOR *p2, bool clip = true);
bool SKPLANECheckFrustumToPolygon(SKPLANE *frustum, SKPOINT *pts, int count, double offset = 0);

void SKPLANELineIntersect(SKPLANE *plane, SKVECTOR *from, SKVECTOR *to, SKVECTOR *out);
bool SKPLANEClipPolygonToFrustum(SKPLANE *frustum, SKPOINT *in, int countIn, SKPOINT *out, int &countOut);

#endif // PLANE_H
