#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "skcore.h"
#include "cmapview.h"
#include "castro.h"


void trfCreateMatrixView(CAstro *ast, mapView_t *mapView, double w, double h);

void trfRaDecToPointNoCorrect(radec_t *rd, SKPOINT *p);
void trfRaDecToPointNoCorrect(radec_t *rd, SKPOINT *p, SKMATRIX *mat);

void trfRaDecToPointCorrectFromTo(const radec_t *rd, SKPOINT *p, double jdFrom, double jdTo);

bool trfProjectLineGetClip(SKPOINT *p1, SKPOINT *p2, bool &bClipped, int &sx, int &sy);
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2);
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2, SKPOINT &out1, SKPOINT &out2);
void trfProjectLineNoCheck(SKPOINT *p1, SKPOINT *p2);
bool trfProjectPoint(SKPOINT *p);
void trfProjectPointNoCheck(SKPOINT *p);
void trfProjectPointNoCheckDbl(SKPOINT *p, double &x, double &y);

bool trfPointOnScr(int x, int y, double rad = 0);
bool trfCheckRDPolygonVis(radec_t *rd, int count);

SKPLANE *trfGetFrustum(void);
SKMATRIX *trfGetTranfMatrix(void);

void trfRestore(void);
void trfSave(void);

int trfGetArcSecToPix(float size);
double trfGetAngleToNPole(double ra, double dec);
double trfGetAngleDegFlipped(double ang);
void trfRotate2dPt(float *tx,float *ty,float cx,float cy,float s,float c);
double trfGetPosAngle(double R1,double D1,double R2,double D2);

void trfSetTransform(SKMATRIX *trans, SKMATRIX *proj, SKMATRIX *view, SKMATRIX *fvp);

void trfConvScrPtToXY(double sx, double sy, double &x, double &y);

#endif // TRANSFORM_H
