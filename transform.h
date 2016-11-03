#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "skcore.h"
#include "cmapview.h"
#include "castro.h"


void trfCreateMatrixView(CAstro *ast, mapView_t *mapView, double w, double h);
void rtfCreateOrthoView(double w, double h, double nearPlane, double farPlane, double scale, const QVector3D &translate, double yaw, double pitch, bool lookAt = false);

void trfRaDecToPointNoCorrect(const radec_t *rd, SKPOINT *p);
void trfRaDecToPointNoCorrect(radec_t *rd, SKPOINT *p, SKMATRIX *mat);

void trfRaDecToPointCorrectFromTo(const radec_t *rd, SKPOINT *p, double jdFrom, double jdTo);

bool trfProjectLine(SKPOINT *p1, SKPOINT *p2);
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2, SKPOINT &out1, SKPOINT &out2);
void trfProjectLineNoCheck(SKPOINT *p1, SKPOINT *p2);
bool trfProjectPoint(SKPOINT *p);
void trfProjectPointNoCheck(SKPOINT *p);
void trfProjectPointNoCheckDbl(SKPOINT *p, double &x, double &y);
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2, QPointF *out);

void trfGetScreenSize(int &width, int &height);
bool trfPointOnScr(int x, int y, double rad = 0);
bool trfCheckRDPolygonVis(radec_t *rd, int count);

void trfGetCenter(double &sx, double &sy);

SKPLANE *trfGetFrustum(void);
SKMATRIX *trfGetTranfMatrix(void);

void trfRestore(void);
void trfSave(void);

int trfGetArcSecToPix(float size);
double trfGetAngleToNPole(double ra, double dec, double epoch = JD2000);
double trfGetAngleToEast(double ra, double dec, double epoch = JD2000);
double trfGetAngleDegFlipped(double ang);
void trfRotate2dPt(float *tx,float *ty,float cx,float cy,float s,float c);
double trfGetPosAngle(double R1,double D1,double R2,double D2);

void trfSetTransform(SKMATRIX *trans, SKMATRIX *proj, SKMATRIX *view, SKMATRIX *fvp);

void trfConvScrPtToXY(double sx, double sy, double &x, double &y);

#endif // TRANSFORM_H
