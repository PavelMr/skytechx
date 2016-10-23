#include <math.h>
#include <string.h>

#include <QtCore>

#include "skmatrix.h"

///////////////////////////////////////////////////////////////////////
void SKMATRIXSet(double m0, double m1, double m3, double m4,
                 double m5, double m6, double m7, double m8,
                 double m9, double m10, double m11, double m12,
                 double m13, double m14, double m15, double m16, SKMATRIX *m)
///////////////////////////////////////////////////////////////////////
{
  m->m_11 = m0; m->m_12 = m1; m->m_13 = m3; m->m_14 = m4;
  m->m_21 = m5; m->m_22 = m6; m->m_23 = m7; m->m_24 = m8;
  m->m_31 = m9; m->m_32 = m10; m->m_33 = m11; m->m_34 = m12;
  m->m_41 = m13; m->m_42 = m14; m->m_43 = m15; m->m_44 = m16;
}

///////////////////////////////////////
SKMATRIX *SKMATRIXIdentity(SKMATRIX *m)
///////////////////////////////////////
{
  m->m_11 = 1.0; m->m_12 = 0.0; m->m_13 = 0.0; m->m_14 = 0.0;
  m->m_21 = 0.0; m->m_22 = 1.0; m->m_23 = 0.0; m->m_24 = 0.0;
  m->m_31 = 0.0; m->m_32 = 0.0; m->m_33 = 1.0; m->m_34 = 0.0;
  m->m_41 = 0.0; m->m_42 = 0.0; m->m_43 = 0.0; m->m_44 = 1.0;

  return(m);
}

/////////////////////////////////////////////
SKMATRIX operator *(SKMATRIX m1, SKMATRIX m2)
/////////////////////////////////////////////
{
  SKMATRIX out;

  SKMATRIXMultiply(&out, &m1, &m2);

  return(out);
}

/////////////////////////////////////////////////////////////////////
SKMATRIX *SKMATRIXMultiply(SKMATRIX *out, SKMATRIX *m1, SKMATRIX *m2)
/////////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      double summ = 0;

      for (int k = 0; k < 4; k++)
      {
        summ += m1->m[i][k] * m2->m[k][j];
      }

      out->m[i][j] = summ;
    }
  }
  return(out);
}


////////////////////////////////////////////////////////////////////////
SKMATRIX *SKMATRIXTranslate(SKMATRIX *out, double x, double y, double z)
////////////////////////////////////////////////////////////////////////
{
  SKMATRIXIdentity(out);

  out->m_41 = x;
  out->m_42 = y;
  out->m_43 = z;

  return(out);
}


////////////////////////////////////////////////////
SKMATRIX *SKMATRIXRotateX(SKMATRIX *out, double ang)
////////////////////////////////////////////////////
{
  SKMATRIXIdentity(out);

  double s = sin(ang);
  double c = cos(ang);

  out->m_22 =  c;
  out->m_23 =  s;
  out->m_32 = -s;
  out->m_33 =  c;

  return(out);
}


////////////////////////////////////////////////////
SKMATRIX *SKMATRIXRotateY(SKMATRIX *out, double ang)
////////////////////////////////////////////////////
{
  SKMATRIXIdentity(out);

  double s = sin(ang);
  double c = cos(ang);

  out->m_11 =  c;
  out->m_13 = -s;
  out->m_31 =  s;
  out->m_33 =  c;

  return(out);
}


////////////////////////////////////////////////////
SKMATRIX *SKMATRIXRotateZ(SKMATRIX *out, double ang)
////////////////////////////////////////////////////
{
  SKMATRIXIdentity(out);

  double s = sin(ang);
  double c = cos(ang);

  out->m_11 =  c;
  out->m_12 =  s;
  out->m_21 = -s;
  out->m_22 =  c;

  return(out);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
SKMATRIX *SKMATRIXProjection(SKMATRIX *out, double fovH, double aspect, double nearPlane, double farPlane)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  memset(out, 0, sizeof(SKMATRIX));

  double t = tan(fovH * 0.5);
  double w = 1 / t;
  double h = 1 / t * aspect;
  double Q = farPlane / (farPlane - nearPlane);

  out->m_11 = w;
  out->m_22 = h;
  out->m_33 = Q;
  out->m_43 = -Q * nearPlane;
  out->m_34 = 1;

  return(out);
}


////////////////////////////////////////////////////////////////////
SKMATRIX *SKMATRIXScale(SKMATRIX *dst, double x, double y, double z)
////////////////////////////////////////////////////////////////////
{
  memset(dst, 0, sizeof(SKMATRIX));

  dst->m_11 = x;
  dst->m_22 = y;
  dst->m_33 = z;
  dst->m_44 = 1;

  return(dst);
}

///////////////////////////////////////////////////////
SKMATRIX *SKMATRIXInverse(SKMATRIX *dst, SKMATRIX *mat)
///////////////////////////////////////////////////////
{
  double    tmp[12]; /* temp array for pairs             */
  double    src[16]; /* array of transpose source matrix */
  double    det;     /* determinant                      */

  /* transpose matrix */
  for (int i = 0; i < 4; i++)
  {
    src[i]        = mat->ma[i*4];
    src[i + 4]    = mat->ma[i*4 + 1];
    src[i + 8]    = mat->ma[i*4 + 2];
    src[i + 12]   = mat->ma[i*4 + 3];
  }

  /* calculate pairs for first 8 elements (cofactors) */
  tmp[0]  = src[10] * src[15];
  tmp[1]  = src[11] * src[14];
  tmp[2]  = src[9]  * src[15];
  tmp[3]  = src[11] * src[13];
  tmp[4]  = src[9]  * src[14];
  tmp[5]  = src[10] * src[13];
  tmp[6]  = src[8]  * src[15];
  tmp[7]  = src[11] * src[12];
  tmp[8]  = src[8]  * src[14];
  tmp[9]  = src[10] * src[12];
  tmp[10] = src[8]  * src[13];
  tmp[11] = src[9]  * src[12];

  /* calculate first 8 elements (cofactors) */
  dst->ma[0]  = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
  dst->ma[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
  dst->ma[1]  = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
  dst->ma[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
  dst->ma[2]  = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
  dst->ma[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
  dst->ma[3]  = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
  dst->ma[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
  dst->ma[4]  = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
  dst->ma[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
  dst->ma[5]  = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
  dst->ma[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
  dst->ma[6]  = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
  dst->ma[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
  dst->ma[7]  = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
  dst->ma[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

  /* calculate pairs for second 8 elements (cofactors) */
  tmp[0]  = src[2]*src[7];
  tmp[1]  = src[3]*src[6];
  tmp[2]  = src[1]*src[7];
  tmp[3]  = src[3]*src[5];
  tmp[4]  = src[1]*src[6];
  tmp[5]  = src[2]*src[5]; //Streaming SIMD Extensions - Inverse of 4x4 Matrix 8
  tmp[6]  = src[0]*src[7];
  tmp[7]  = src[3]*src[4];
  tmp[8]  = src[0]*src[6];
  tmp[9]  = src[2]*src[4];
  tmp[10] = src[0]*src[5];
  tmp[11] = src[1]*src[4];

  /* calculate second 8 elements (cofactors) */
  dst->ma[8]  = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
  dst->ma[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
  dst->ma[9]  = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
  dst->ma[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
  dst->ma[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
  dst->ma[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
  dst->ma[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
  dst->ma[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
  dst->ma[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
  dst->ma[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
  dst->ma[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
  dst->ma[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
  dst->ma[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
  dst->ma[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
  dst->ma[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
  dst->ma[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

  /* calculate determinant */
  det = src[0]*dst->ma[0]+src[1]*dst->ma[1]+src[2]*dst->ma[2]+src[3]*dst->ma[3];

  /* calculate matrix inverse */
  det = 1/det;
  for (int j = 0; j < 16; j++)
    dst->ma[j] *= det;

  return(dst);
}


SKMATRIX *SKMATRIXOrtho(SKMATRIX *out, double w, double h, double nearPlane, double farPlane)
{
  SKMATRIXIdentity(out);

  out->m_11 =  2 / w;
  out->m_22 =  2 / h;
  //out->m_33 =  1  / (farPlane / nearPlane);
  //out->m_43 =  -nearPlane / (farPlane / nearPlane);
  out->m_33 =  1  / (nearPlane / farPlane);
  out->m_43 =  nearPlane / (nearPlane / farPlane);
  out->m_44 =  1;

  return out;
}

















