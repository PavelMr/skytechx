#include <math.h>

#include "plane.h"
#include "vecmath.h"
#include "const.h"

#define FRUSTUM_COUNT   m_numFrustums

extern int m_numFrustums;


//////////////////////////////////////////////////////////////////////////////
SKPLANE *SKPLANEFromPoint(SKPLANE *out, SKVECTOR *a, SKVECTOR *b, SKVECTOR *c)
//////////////////////////////////////////////////////////////////////////////
{
  SKVECTOR d1, d2;
  SKVECTOR normal;

  SKVecSub(&d1, b, a);
  SKVecSub(&d2, c, a);

  SKVecCross(&normal, &d1, &d2);
  SKVecNormalize(&normal, &normal);

  out->dist =  (a->x * normal.x +
                 a->y * normal.y +
                 a->z * normal.z);

  out->x = normal.x;
  out->y = normal.y;
  out->z = normal.z;

  return(out);
}

//////////////////////////////////////////////////////////////////
bool SKPLANECheckFrustumToPoint(SKPLANE *frustum, SKVECTOR *point)
//////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    double dist = point->x * frustum[i].x +
                  point->y * frustum[i].y +
                  point->z * frustum[i].z - frustum[i].dist;
    if (dist < 0)
      return(false);
  }
  return(true);
}

//////////////////////////////////////////////////////////////////////////////////
bool SKPLANECheckFrustumToSphere(SKPLANE *frustum, SKVECTOR *point, double radius)
//////////////////////////////////////////////////////////////////////////////////
{
  // angle to size
  radius = tan(radius);

  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    double dist = point->x * frustum[i].x +
                  point->y * frustum[i].y +
                  point->z * frustum[i].z - frustum[i].dist;
    if (dist < -radius)
      return(false);
  }
  return(true);
}

//////////////////////////////////////////////////////////////////////////////////////
// perform line intersection
void SKPLANELineIntersect(SKPLANE *plane, SKVECTOR *from, SKVECTOR *to, SKVECTOR *out)
//////////////////////////////////////////////////////////////////////////////////////
{
  SKVECTOR ba;

  SKVecSub(&ba, to, from);

  double dotA  = SKVecDot((SKVECTOR *)plane, from);
  double dotBA = SKVecDot((SKVECTOR *)plane, &ba);
  double f = (plane->dist - dotA) / dotBA;

  out->x = from->x + f * ba.x;
  out->y = from->y + f * ba.y;
  out->z = from->z + f * ba.z;
}


///////////////////////////////////////////////////////////////////////////////////////////
bool SKPLANECheckFrustumToPolygon(SKPLANE *frustum, SKPOINT *pts, int count, double offset)
///////////////////////////////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    int p;
    for (p = 0; p < count; p++)
    {
      double dist = pts[p].w.x * frustum[i].x +
                    pts[p].w.y * frustum[i].y +
                    pts[p].w.z * frustum[i].z - frustum[i].dist;
      if (dist > offset)
        break;
    }
    if (p == count)
      return(false);
  }
  return(true);
}

///////////////////////////////////////////////////////////////////////////////////////
// with line clipping
bool SKPLANECheckFrustumToLine(SKPLANE *frustum, SKVECTOR *p1, SKVECTOR *p2, bool clip)
///////////////////////////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    double dist1 = p1->x * frustum[i].x +
                   p1->y * frustum[i].y +
                   p1->z * frustum[i].z - frustum[i].dist;

    double dist2 = p2->x * frustum[i].x +
                   p2->y * frustum[i].y +
                   p2->z * frustum[i].z - frustum[i].dist;

    if(dist1 < 0 && dist2 < 0)
      return(false); // completely outside
    if(dist1 * dist2 > 0)
      continue; // both points are inside, no clip

    if (clip)
    {
      SKVECTOR out;

      SKPLANELineIntersect(&frustum[i], p1, p2, &out);

      if(dist1 < 0)
      {
        p1->x = out.x;
        p1->y = out.y;
        p1->z = out.z;
      }
       else
      {
        p2->x = out.x;
        p2->y = out.y;
        p2->z = out.z;
      }
    }
  }

  return(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
void SKPLANEClipPolygonToPlane(SKPLANE *plane, SKPOINT *in, int countIn, SKPOINT *out, int &countOut)
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
  int location[MAX_POLYGON_PTS];
  int positive = 0;
  int negative = 0;

  for (int a = 0; a < countIn; a++)
  {
    double d = in[a].w.x * plane->x +
               in[a].w.y * plane->y +
               in[a].w.z * plane->z - plane->dist;

    if (d > PLANE_EPSILON)
    {
      location[a] = polygonInterior;
      positive++;
    }
    else
    {
      if (d < -PLANE_EPSILON)
      {
        location[a] = polygonExterior;
        negative++;
      }
      else
      {
        location[a] = polygonBoundary;
      }
    }
  }

  if (negative == 0)
  {
    for (int a = 0; a < countIn; a++)
      out[a] = in[a];

    countOut = countIn;
    return;
  }
  else if (positive == 0)
  {
    countOut = 0;
    return;
  }

  int count = 0;
  int previous = countIn - 1;

  for (int index = 0; index < countIn; index++)
  {
    long loc = location[index];

    if (loc == polygonExterior)
    {
      if (location[previous] == polygonInterior)
      {
        SKPOINT  v1 = in[previous];
        SKPOINT  v2 = in[index];

        SKPLANELineIntersect(plane, &v1.w, &v2.w, &out[count++].w);
      }
    }
    else
    {
      SKPOINT  v1 = in[index];

      if ((loc == polygonInterior) && (location[previous] == polygonExterior))
      {
        SKPOINT  v2 = in[previous];

        SKPLANELineIntersect(plane, &v1.w, &v2.w, &out[count++].w);
      }
      out[count++] = v1;
    }
    previous = index;
  }

  countOut = count;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SKPLANEClipPolygonToFrustum(SKPLANE *frustum, SKPOINT *in, int countIn, SKPOINT *out, int &countOut)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  SKPOINT newOut[MAX_POLYGON_PTS];
  int     newOutCount;

  SKPLANEClipPolygonToPlane(&frustum[0], in, countIn, out, countOut);
  SKPLANEClipPolygonToPlane(&frustum[1], out, countOut, newOut, newOutCount);
  SKPLANEClipPolygonToPlane(&frustum[2], newOut, newOutCount, out, countOut);
  SKPLANEClipPolygonToPlane(&frustum[3], out, countOut, newOut, newOutCount);
  SKPLANEClipPolygonToPlane(&frustum[4], newOut, newOutCount, out, countOut);

  return(countOut >= 3);
}

