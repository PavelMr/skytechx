#ifndef CGSCREG_H
#define CGSCREG_H

#include "skcore.h"
#include <QtCore>

#define NUM_GSC_REGS      9537

#pragma pack(4)

typedef struct
{  
  SKVECTOR p[4];

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
    void resetRegion();
    void setRegion(int region);
    void addPoint(const radec_t &rd);
    void createRegion(int region);
    void createOcTree(void);

    BBox        gscRegionBBox[NUM_GSC_REGS];
    gscRegion_t gscRegionSector[NUM_GSC_REGS];
    uchar       rendered[NUM_GSC_REGS];

  protected:    
    void createOcTreeRec(regNode_t *node, int depth);
    regNode_t *createNode(const SKVECTOR *pos, const SKVECTOR &size);
    void getVisibleRec(regNode_t *node);

    regNode_t  *m_head;    

    SKPLANE     *m_frustum;
    QList <int> *m_visList;

    double m_raMin;
    double m_raMax;
    double m_decMin;
    double m_decMax;
};

extern CGSCReg cGSCReg;

#endif // CGSCREG_H
