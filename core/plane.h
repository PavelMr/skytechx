/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

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
