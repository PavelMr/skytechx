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

#pragma once

#include <math.h>

#include "plane.h"
#include "vecmath.h"

#include <QDebug>

#define BMAX     999999
#define BMIN    -999999


typedef struct BBox
{
  public:
    BBox()
    {
      reset();
    }

    void reset(void)
    {
      mins[0] = BMAX;
      mins[1] = BMAX;
      mins[2] = BMAX;
      maxs[0] = BMIN;
      maxs[1] = BMIN;
      maxs[2] = BMIN;
    }

    void offset(double x, double y, double z)
    {
      mins[0] += x;
      mins[1] += y;
      mins[2] += z;

      maxs[0] += x;
      maxs[1] += y;
      maxs[2] += z;
    }

    void offset(double *off)
    {
      mins[0] += off[0];
      mins[1] += off[1];
      mins[2] += off[2];

      maxs[0] += off[0];
      maxs[1] += off[1];
      maxs[2] += off[2];
    }

    void debug()
    {
      qDebug() << mins[0] << mins[1] << mins[2];
      qDebug() << maxs[0] << maxs[1] << maxs[2];
    }

    double size(int i)
    {
      return(maxs[i] - mins[i]);
    }

    void center(double *p)
    {
      p[0] = mins[0] + 0.5f * (maxs[0] - mins[0]);
      p[1] = mins[1] + 0.5f * (maxs[1] - mins[1]);
      p[2] = mins[2] + 0.5f * (maxs[2] - mins[2]);
    }

    bool isPtInside(double *p)
    {
      if (p[0] < mins[0] || p[0] > maxs[0]) return(false);
      if (p[1] < mins[1] || p[1] > maxs[1]) return(false);
      if (p[2] < mins[2] || p[2] > maxs[2]) return(false);

      return(true);
    }

    bool isPtInside2D(double *p)
    {
      if (p[0] < mins[0] || p[0] > maxs[0]) return(false);
      if (p[2] < mins[2] || p[2] > maxs[2]) return(false);

      return(true);
    }

    bool intersect(BBox &other)
    {
      return (mins[0] < other.maxs[0]) && (maxs[0] > other.mins[0]) &&
             (mins[1] < other.maxs[1]) && (maxs[1] > other.mins[1]) &&
             (mins[2] < other.maxs[2]) && (maxs[2] > other.mins[2]);
    }

    bool intersect2d(BBox &other)
    {
      return (mins[0] < other.maxs[0]) && (maxs[0] > other.mins[0]) &&
             (mins[2] < other.maxs[2]) && (maxs[2] > other.mins[2]);
    }

    // true = front, other false
    bool planeCheck(SKPLANE *p)
    {
      double fDist =  ((p->x < 0) ? mins[0] : maxs[0]) * p->x +
                      ((p->y < 0) ? mins[1] : maxs[1]) * p->y +
                      ((p->z < 0) ? mins[2] : maxs[2]) * p->z - p->dist;

      return fDist >= 0.f;
    }

    bool checkFrustum(SKPLANE *p)
    {
      for (int i = 0; i < 5; i++)
      {
        if (!planeCheck(&p[i]))
          return(false);
      }
      return(true);
    }

    void expand(double size)
    {
      mins[0] -= size;
      mins[1] -= size;
      mins[2] -= size;

      maxs[0] += size;
      maxs[1] += size;
      maxs[2] += size;
    }

    void addPt(double *p)
    {
      if (p[0] < mins[0]) mins[0] = p[0];
      if (p[1] < mins[1]) mins[1] = p[1];
      if (p[2] < mins[2]) mins[2] = p[2];

      if (p[0] > maxs[0]) maxs[0] = p[0];
      if (p[1] > maxs[1]) maxs[1] = p[1];
      if (p[2] > maxs[2]) maxs[2] = p[2];
    };

    void addPt(double *p, double size)
    {
      if (p[0] - size < mins[0]) mins[0] = p[0] - size;
      if (p[1] - size < mins[1]) mins[1] = p[1] - size;
      if (p[2] - size < mins[2]) mins[2] = p[2] - size;

      if (p[0] + size > maxs[0]) maxs[0] = p[0] + size;
      if (p[1] + size > maxs[1]) maxs[1] = p[1] + size;
      if (p[2] + size > maxs[2]) maxs[2] = p[2] + size;
    };

    void addPt(double x, double y, double z)
    {
      if (x < mins[0]) mins[0] = x;
      if (y < mins[1]) mins[1] = y;
      if (z < mins[2]) mins[2] = z;

      if (x > maxs[0]) maxs[0] = x;
      if (y > maxs[1]) maxs[1] = y;
      if (z > maxs[2]) maxs[2] = z;
    };

    void add(BBox *b)
    {
      addPt(b->mins[0], b->mins[1], b->mins[2]);
      addPt(b->maxs[0], b->maxs[1], b->maxs[2]);
    };

    double mins[3];
    double maxs[3];
} BBox;
