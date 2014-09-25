#ifndef CGSCREG_H
#define CGSCREG_H

#include "skcore.h"
#include <QtCore>

#define NUM_GSC_REGS      9537

#pragma pack(4)

typedef struct
{
  double p[4][3];
  double RaMin, DecMin;
  double RaMax, DecMax;
} gscRegion_t;

#pragma pack()

typedef struct regNode_s
{
  struct regNode_s *child[8];
  BBox              bbox;
  QList <int>       tRegList;
} regNode_t;


class CGSCReg
{
  public:
    CGSCReg();
    void loadRegions(void);
    void getVisibleRegions(QList <int> *list, SKPLANE *frustum);
    bool isRegionVisible(int reg, SKPLANE *frustum);
    gscRegion_t *getRegion(int reg);

    BBox        gscRegionBBox[NUM_GSC_REGS];
    gscRegion_t gscRegionSector[NUM_GSC_REGS];
    uchar       rendered[NUM_GSC_REGS];

  protected:
    void createOcTree(void);
    void createOcTreeRec(regNode_t *node, int depth);
    regNode_t *createNode(const SKVECTOR *pos, const SKVECTOR &size);
    void getVisibleRec(regNode_t *node);

    regNode_t  *m_head;
    int         test[NUM_GSC_REGS];

    SKPLANE     *m_frustum;
    QList <int> *m_visList;
};

extern CGSCReg cGSCReg;

#endif // CGSCREG_H
