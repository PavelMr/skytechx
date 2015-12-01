#include <math.h>

#include "vecmath.h"
#include "skmatrix.h"


/////////////////////////////////
double SKVECLength(SKVECTOR *vec)
/////////////////////////////////
{
  double d = vec->x * vec->x +
             vec->y * vec->y +
             vec->z * vec->z;

  return(sqrt(d));
}


/////////////////////////////////////
double SKVECLengthSqrt(SKVECTOR *vec)
/////////////////////////////////////
{
  double d = vec->x * vec->x +
             vec->y * vec->y +
             vec->z * vec->z;

  return(d);
}

//////////////////////////////////////////////////////
SKVECTOR *SKVecNormalize(SKVECTOR *out, SKVECTOR *vec)
//////////////////////////////////////////////////////
{
  double len = SKVECLength(vec);
  double iLen;

  if (len == 0)
  {
    out->x = 0;
    out->y = 0;
    out->z = 0;

    return(out);
  }
  else
  {
    iLen = 1 / len;
  }

  out->x = vec->x * iLen;
  out->y = vec->y * iLen;
  out->z = vec->z * iLen;

  return(out);
}


/////////////////////////////////////////////////////////////
SKVECTOR *SKVecAdd(SKVECTOR *out, SKVECTOR *a1, SKVECTOR *a2)
/////////////////////////////////////////////////////////////
{
  out->x = a1->x + a2->x;
  out->y = a1->y + a2->y;
  out->z = a1->z + a2->z;

  return(out);
}


/////////////////////////////////////////////////////////////
SKVECTOR *SKVecSub(SKVECTOR *out, SKVECTOR *a1, SKVECTOR *a2)
/////////////////////////////////////////////////////////////
{
  out->x = a1->x - a2->x;
  out->y = a1->y - a2->y;
  out->z = a1->z - a2->z;

  return(out);
}


///////////////////////////////////////////
double SKVecDot(SKVECTOR *a1, SKVECTOR *a2)
///////////////////////////////////////////
{
  double d = a1->x * a2->x +
             a1->y * a2->y +
             a1->z * a2->z;
  return(d);
}


///////////////////////////////////////////////////////////////
SKVECTOR *SKVecCross(SKVECTOR *out, SKVECTOR *a1, SKVECTOR *a2)
///////////////////////////////////////////////////////////////
{
  out->x = a1->y * a2->z - a1->z * a2->y;
  out->y = a1->z * a2->x - a1->x * a2->z;
  out->z = a1->x * a2->y - a1->y * a2->x;

  return(out);
}

/////////////////////////////////////////////////////////////////////
SKVECTOR *SKVECTransform(SKVECTOR *out, SKVECTOR *vec, SKMATRIX *mat)
/////////////////////////////////////////////////////////////////////
{
  double px = vec->x * mat->m_11 +
              vec->y * mat->m_21 +
              vec->z * mat->m_31 +
                       mat->m_41;

  double py = vec->x * mat->m_12 +
              vec->y * mat->m_22 +
              vec->z * mat->m_32 +
                       mat->m_42;

  double pz = vec->x * mat->m_13 +
              vec->y * mat->m_23 +
              vec->z * mat->m_33 +
                       mat->m_43;

  double pw = vec->x * mat->m_14 +
              vec->y * mat->m_24 +
              vec->z * mat->m_34 +
                       mat->m_44;

  double iw = 1 / pw;

  out->x = px * iw;
  out->y = py * iw;
  out->z = pz * iw;

  return(out);
}


//////////////////////////////////////////////////////////////////////
SKVECTOR *SKVECTransform3(SKVECTOR *out, SKVECTOR *vec, SKMATRIX *mat)
//////////////////////////////////////////////////////////////////////
{
  double px = vec->x * mat->m_11 +
              vec->y * mat->m_21 +
              vec->z * mat->m_31;

  double py = vec->x * mat->m_12 +
              vec->y * mat->m_22 +
              vec->z * mat->m_32;

  double pz = vec->x * mat->m_13 +
              vec->y * mat->m_23 +
              vec->z * mat->m_33;

  out->x = px;
  out->y = py;
  out->z = pz;

  return(out);
}
