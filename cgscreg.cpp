#include "cgscreg.h"

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
        test[i]++;
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
  memset(test, 0, sizeof(test));

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
      gscRegionBBox[i].addPt(gscRegionSector[i].p[j][0],
                             gscRegionSector[i].p[j][1],
                             gscRegionSector[i].p[j][2]);
    }
  }

  QElapsedTimer tm;

  tm.start();

  createOcTree();  
}

