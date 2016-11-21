#include "cgscreg.h"
#include "transform.h"

CGSCReg cGSCReg;

extern int g_ocTreeDepth;

static int mem = 0;

//////////////////
CGSCReg::CGSCReg()
//////////////////
{
  m_head = NULL;
}

///////////////////////////////////////////////////////////////////
void CGSCReg::getVisibleRegions(QList<int> *list, SKPLANE *frustum)
///////////////////////////////////////////////////////////////////
{  
  m_frustum = frustum;
  m_visList = list;
  getVisibleRec(m_head);  
}

////////////////////////////////////////////////////////
bool CGSCReg::isRegionVisible(int reg, SKPLANE *frustum)
////////////////////////////////////////////////////////
{  
  return(gscRegionBBox[reg].checkFrustum(frustum));
}

////////////////////////////////////////
gscRegion_t *CGSCReg::getRegion(int reg)
////////////////////////////////////////
{
  return(&gscRegionSector[reg]);
}

void CGSCReg::resetRegion()
{
  m_raMin = 9999999;
  m_raMax = -9999999;
  m_decMin = 9999999;
  m_decMax = -9999999;
}

void CGSCReg::addPoint(const radec_t &rd)
{
  if (rd.Ra < m_raMin) m_raMin = rd.Ra;
  if (rd.Ra > m_raMax) m_raMax = rd.Ra;
  if (rd.Dec < m_decMin) m_decMin = rd.Dec;
  if (rd.Dec > m_decMax) m_decMax = rd.Dec;
}

void CGSCReg::createRegion(int region)
{
  SKPOINT p0;
  SKPOINT p1;
  SKPOINT p2;
  SKPOINT p3;

  m_decMax += 0.001;
  m_decMin -= 0.001;
  m_raMax += 0.001;
  m_raMin -= 0.001;

  trfRaDecToPointNoCorrect(&radec_t(m_raMin, m_decMin), &p1);
  trfRaDecToPointNoCorrect(&radec_t(m_raMin, m_decMax), &p0);
  trfRaDecToPointNoCorrect(&radec_t(m_raMax, m_decMin), &p2);
  trfRaDecToPointNoCorrect(&radec_t(m_raMax, m_decMax), &p3);

  gscRegionSector[region].DecMax = m_decMax;
  gscRegionSector[region].DecMin = m_decMin;
  gscRegionSector[region].RaMax = m_raMax;
  gscRegionSector[region].RaMin = m_raMin;

  gscRegionSector[region].p[0].x = p0.w.x;
  gscRegionSector[region].p[0].y = p0.w.y;
  gscRegionSector[region].p[0].z = p0.w.z;

  gscRegionSector[region].p[1].x = p1.w.x;
  gscRegionSector[region].p[1].y = p1.w.y;
  gscRegionSector[region].p[1].z = p1.w.z;

  gscRegionSector[region].p[2].x = p2.w.x;
  gscRegionSector[region].p[2].y = p2.w.y;
  gscRegionSector[region].p[2].z = p2.w.z;

  gscRegionSector[region].p[3].x = p3.w.x;
  gscRegionSector[region].p[3].y = p3.w.y;
  gscRegionSector[region].p[3].z = p3.w.z;

  for (int j = 0; j < 4; j++)
  {
    gscRegionBBox[region].addPt(gscRegionSector[region].p[j].x,
                                gscRegionSector[region].p[j].y,
                                gscRegionSector[region].p[j].z);
  }
}

/////////////////////////////////////////////////////////////////////////
regNode_t *CGSCReg::createNode(const SKVECTOR *pos, const SKVECTOR &size)
/////////////////////////////////////////////////////////////////////////
{
  regNode_t *node = new regNode_t;
  mem += sizeof(regNode_t);

  for (int i = 0; i < 8; i++)
  {
    node->child[i] = NULL;
  }

  node->bbox.reset();
  node->bbox.addPt((double *)pos);
  node->bbox.addPt(pos->x + size.x, pos->y + size.y, pos->z + size.z);

  return(node);
}

////////////////////////////////////////////
void CGSCReg::getVisibleRec(regNode_t *node)
////////////////////////////////////////////
{
  if (node->child[0] == NULL)
  { // add to list;
    m_visList->append(node->tRegList);
    return;
  }

  for (int i = 0; i < 8; i++)
  {
    if (node->child[i]->bbox.checkFrustum(m_frustum))
    { // visible
      getVisibleRec(node->child[i]);
    }
  }
}

/////////////////////////////////////////////////////////
void CGSCReg::createOcTreeRec(regNode_t *node, int depth)
/////////////////////////////////////////////////////////
{
  if (depth == g_ocTreeDepth)
  {        
    for (int i = 0; i < NUM_GSC_REGS; i++)
    {
      if (node->bbox.intersect(gscRegionBBox[i]))
      {        
        node->tRegList.append(i);        
      }
    }
    return;
  }

  SKVECTOR size;

  size.x = node->bbox.size(0) * 0.5f;
  size.y = node->bbox.size(1) * 0.5f;
  size.z = node->bbox.size(2) * 0.5f;

  SKVECTOR vec0(node->bbox.mins[0], node->bbox.mins[1], node->bbox.mins[2]);
  SKVECTOR vec1(node->bbox.mins[0], node->bbox.mins[1], node->bbox.mins[2] + size.z);
  SKVECTOR vec2(node->bbox.mins[0] + size.x, node->bbox.mins[1], node->bbox.mins[2] + size.z);
  SKVECTOR vec3(node->bbox.mins[0] + size.x, node->bbox.mins[1], node->bbox.mins[2]);
  SKVECTOR vec4(node->bbox.mins[0], node->bbox.mins[1] + size.y, node->bbox.mins[2]);
  SKVECTOR vec5(node->bbox.mins[0], node->bbox.mins[1] + size.y, node->bbox.mins[2] + size.z);
  SKVECTOR vec6(node->bbox.mins[0] + size.x, node->bbox.mins[1] + size.y, node->bbox.mins[2] + size.z);
  SKVECTOR vec7(node->bbox.mins[0] + size.x, node->bbox.mins[1] + size.y, node->bbox.mins[2]);

  node->child[0] = createNode(&vec0, size);
  node->child[1] = createNode(&vec1, size);
  node->child[2] = createNode(&vec2, size);
  node->child[3] = createNode(&vec3, size);

  node->child[4] = createNode(&vec4, size);
  node->child[5] = createNode(&vec5, size);
  node->child[6] = createNode(&vec6, size);
  node->child[7] = createNode(&vec7, size);

  for (int i = 0; i < 8; i++)
  {
    createOcTreeRec(node->child[i], depth + 1);
  }
}


////////////////////////////////
void CGSCReg::createOcTree(void)
////////////////////////////////
{  
  SKVECTOR vec(-1.1, -1.1, -1.1);
  m_head = createNode(&vec, SKVECTOR(2.2, 2.2, 2.2));
  createOcTreeRec(m_head, 0);
}


///////////////////////////////
void CGSCReg::loadRegions(void)
///////////////////////////////
{
  int count;

  SkFile f("../data/common/gscregions.dat");

  f.open(SkFile::ReadOnly);

  f.read((char *)&count, sizeof(int));
  f.read((char *)gscRegionSector, sizeof(gscRegionSector));

  f.close();

  for (int i = 0; i < NUM_GSC_REGS; i++)
  {
    gscRegionBBox[i].reset();
    for (int j = 0; j < 4; j++)
    {
      gscRegionBBox[i].addPt(gscRegionSector[i].p[j].x,
                             gscRegionSector[i].p[j].y,
                             gscRegionSector[i].p[j].z);
    }
  }

  QElapsedTimer tm;

  tm.start();

  createOcTree();  
}

