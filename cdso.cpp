#include "cdso.h"
#include "mapobj.h"
#include "setting.h"
#include "cconsole.h"
#include "smartlabeling.h"

// FIXME: zrusit duplikaty
// M47, IC 5146 =Sh2-125

CDso  cDSO;

extern bool g_showDSOShapes;
extern bool g_showLabels;

////////////
CDso::CDso()
////////////
{
  dsoHead.numDso = 0;
  m_minSize = MIN_DSO_SIZE;
}

/////////////
CDso::~CDso()
/////////////
{
  for (int i = 0; i < tShapeList.count(); i++)
  {
    delete tShapeList[i];
  }

  tShapeList.clear();

  free(dso);
  free(dsoClass);
  free(dsoCats);
}

////////////////////////////////////////////////////////
static int dsoSort(const void *elem1, const void *elem2)
////////////////////////////////////////////////////////
{
  dso_t *p1 = (dso_t *)elem1;
  dso_t *p2 = (dso_t *)elem2;

  if (p1->sx > p2->sx)
    return(-1);

  if (p1->sx < p2->sx)
    return(1);

  return (0);
}


/////////////////
void CDso::load()
/////////////////
{
  SkFile pf("../data/dso/dso.dat");

  if (!pf.open(SkFile::ReadOnly))
    return;

  pf.read((char *)&dsoHead.id, 4);
  pf.read((char *)&dsoHead.numDso, sizeof(qint32));
  pf.read((char *)&dsoHead.textSegSize, sizeof(qint32));
  pf.read((char *)&dsoHead.galClassSize, sizeof(qint32));
  pf.read((char *)&dsoHead.catNamesSize, sizeof(qint32));

  dso =      (dso_t *)malloc(sizeof(dso_t) * dsoHead.numDso);
  dsoNames = (char *)malloc(dsoHead.textSegSize);
  dsoClass = (char *)malloc(dsoHead.galClassSize);
  dsoCats  = (char *)malloc(dsoHead.catNamesSize);

  pf.read((char *)dsoNames, dsoHead.textSegSize);
  pf.read((char *)dsoClass, dsoHead.galClassSize);
  pf.read((char *)dsoCats, dsoHead.catNamesSize);

  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {
    pf.read((char *)&dso[i].nameOffs, sizeof(qint32));
    pf.read((char *)&dso[i].rd.Ra, sizeof(double));
    pf.read((char *)&dso[i].rd.Dec, sizeof(double));
    pf.read((char *)&dso[i].mag, sizeof(signed short));
    pf.read((char *)&dso[i].pa, sizeof(unsigned short));
    pf.read((char *)&dso[i].sx, sizeof(quint32));
    pf.read((char *)&dso[i].sy, sizeof(quint32));
    pf.read((char *)&dso[i].type, sizeof(unsigned char));
    pf.read((char *)&dso[i].cataloque, sizeof(unsigned char));
    pf.read((char *)&dso[i].shape, sizeof(unsigned short));
    pf.read((char *)&dso[i].galType, sizeof(unsigned short));

    dso[i].show = true;

    rangeDbl(&dso[i].rd.Ra, MPI2);
    if (dso[i].sx > 0 && dso[i].sy == 0)
    {
      dso[i].sy = dso[i].sx;
    }
  }
  pf.close();

  // sort dso by size
  qsort(dso, dsoHead.numDso, sizeof(dso_t), dsoSort);

  createSectors();  

  // assign to sectors
  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {
    int ra  = (int)((RAD2DEG(dso[i].rd.Ra)) / DSO_SEG_SIZE);
    int dec = (int)((RAD2DEG(dso[i].rd.Dec) + 90.) / DSO_SEG_SIZE);
    tDsoSectors[(int)dec][(int)ra].append(i);
  }

  // create name map list
  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {    
    namesMap[dso[i].nameOffs].append(cDSO.getNameInt(&dso[i]));
    Q_ASSERT(namesMap[dso[i].nameOffs].size() > 0);
  }
  free(dsoNames);

  loadShapes();
  loadNames();

  //qDebug() << getCatalogue(&dso[dsoHead.numDso - 10]);
}


////////////////////////////////////////
QString CDso::getCommonName(dso_t *pDso)
////////////////////////////////////////
{
  QString str;

  for (int i = 0; i < tDsoCommonNames.count(); i++)
  {
    for (int n = 0; n < 20; n++)
    {
      QString name = getName(pDso, n);
      if (name.length() == 0)
        break;

      if (compareName(name.toLatin1().data(), tDsoCommonNames[i].catName))
      {
        str = tDsoCommonNames[i].commonName;
        break;
      }
    }
  }
  return(str);
}


//////////////////////////
void CDso::loadNames(void)
//////////////////////////
{
  SkFile f("../data/dso/names.dat");
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QString str;
    QTextStream s(&f);

    while (1)
    {
      dsoCommonName_t i;
      int             idx;

      str = s.readLine();
      if (str.isNull())
        break;

      idx = str.indexOf("|");

      if (idx != -1)
      {
        strcpy(i.catName, str.mid(0, idx).simplified().toLocal8Bit().data());
        i.commonName = str.mid(idx + 1).simplified();

        tDsoCommonNames.append(i);
      }
    }
  }
}



///////////////////////////////////////
QString CDso::getCatalogue(dso_t *pDso)
///////////////////////////////////////
{
  return getCatalogue(pDso->cataloque);
}


/////////////////////////////////////
QString CDso::getCatalogue(int index)
/////////////////////////////////////
{
  char *p = dsoCats;
  QString str;
  static char tmp[256];
  char *ptmp = tmp;
  qint32 idx = 0;
  qint32 cnt = 0;

  while (1)
  {
    while (*p != '\0')
    {
      *ptmp = *p;
      ptmp++;
      p++;
      cnt++;
    }
    if (idx == index)
    {
      *ptmp = '\0';
      str = tmp;
      return(str);
    }
    ptmp = tmp;
    p++;
    cnt++;
    idx++;
    if (cnt >= dsoHead.catNamesSize)
      break;
  }

  return("");
}


///////////////////////////////////
QString CDso::getClass(dso_t *pDso)
///////////////////////////////////
{
  char *p = dsoClass + pDso->galType;
  QString str;
  static char tmp[256];
  char *ptmp = tmp;

  while (*p != '\0')
  {
    *ptmp = *p;
    ptmp++;
    p++;
  }

  *ptmp = '\0';
  str = tmp;

  return(str);
}

QStringList CDso::getCommonNameList()
{
  QStringList list;

  for (int i = 0; i < tDsoCommonNames.count(); i++)
  {
    list.append(tDsoCommonNames[i].commonName);
  }

  return list;
}


/////////////////////////////////////////////
QString CDso::getTypeName(int type, bool &ok)
/////////////////////////////////////////////
{
  ok = true;

  switch (type)
  {
    case DSOT_UNKNOWN:
      return(tr("Unknown object"));
    case DSOT_NEBULA:
      return(tr("Nebula"));
    case DSOT_BRIGHT_NEB:
      return(tr("Bright nebula"));
    case DSOT_DARK_NEB:
      return(tr("Dark nebula"));
    case DSOT_NGC_DUPP:
      return(tr("NGC Duplicate"));
    case DSOT_PLN_NEBULA:
      return(tr("Planetary nebula"));
    case DSOT_OPEN_CLUSTER:
      return(tr("Open cluster"));
    case DSOT_OPEN_CLS_NEB:
      return(tr("Open cluster with nebulosity"));
    case DSOT_ASTERISM:
      return(tr("Asterism"));
    case DSOT_STARS:
      return(tr("Double star"));
    case DSOT_MILKY_SC:
      return(tr("Milky Way star cloud"));
    case DSOT_GALAXY:
      return(tr("Galaxy"));
    case DSOT_GALAXY_CLD:
      return(tr("Part of galaxy"));
    case DSOT_GAL_CLUSTER:
      return(tr("Cluster of galaxies"));
    case DSOT_STAR:
      return(tr("Star"));
    case DSOT_SUPER_REM:
      return(tr("Supernova remnant"));
    case DSOT_QUASAR:
      return(tr("Quasar"));
    case DSOT_GLOB_CLUSTER:
      return(tr("Globular cluster"));
    case DSOT_OTHER:
      return(tr("Other object"));
    default:
      ok = false;
      return(tr("Invalid object"));
  }
  ok = false;
  return(tr("Invalid object"));
}


//////////////////////////////////////////////////////////
int CDso::findDSO(char *pszName, dso_t **pDso, int &index)
/////////////////////////////////////////////////////////
{
  if (pszName[0] == '\0')
    return(-1);

  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      QString pName = getName(&dso[i], j);
      if (pName.length() == 0)
      {
        break;
      }
      if (compareName(pName.toLatin1().data(), pszName))
      {
        *pDso = &dso[i];
        index = i;
        return(i);
      }
    }
  }

  for (int c = 0; c < tDsoCommonNames.count(); c++)
  {
    if (tDsoCommonNames[c].commonName.compare(pszName, Qt::CaseInsensitive) == 0)
    {
      for (qint32 i = 0; i < dsoHead.numDso; i++)
      {
        for (int j = 0; j < 20; j++)
        {
          QString pName = getName(&dso[i], j);
          if (pName.length() == 0)
            break;
          if (compareName(pName.toLatin1().data(), tDsoCommonNames[c].catName))
          {
            *pDso = &dso[i];
            index = i;
            return(i);
          }
        }
      }
    }
  }

  index = -1;
  return(-1);
}

int CDso::findDSOFirstName(char *pszName)
{
  if (pszName[0] == '\0')
    return(-1);

  QString name = pszName;

  name.remove(" ");

  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {
    QString pName = getName(&dso[i], 0);

    if (pName.compare(name, Qt::CaseInsensitive) == 0)
    {
      return(i);
    }
  }

  return -1;
}


///////////////////////
void CDso::loadShapes()
///////////////////////
{
  int idx = 0;
  QDir dir("../data/dso/shapes/", "*.dat");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.count(); i++)
  {
    dso_t *pDso;
    QFileInfo fi = list.at(i);
    int index;
    if (findDSO(fi.baseName().toLatin1().data(), &pDso, index) >= 0)
    {      
      CShape *shp = new CShape;
      if (shp->load(fi.filePath()))
      {
        pDso->shape = idx;
        tShapeList.append(shp);
        idx++;
      }
      else
      {
        delete shp;
      }
    }
  }
}

//////////////////////////
void CDso::createSectors()
//////////////////////////
{
  double yy;
  double xx = 0;
  SKPOINT p;
  radec_t rd;

  yy = -11;
  for (int y = 0; y < NUM_DSO_SEG_Y; y++, yy++)
  {
    for (int x = 0; x < NUM_DSO_SEG_X; x++, xx++)
    {
      rd.Ra = DEG2RAD(xx * DSO_SEG_SIZE);
      rd.Dec = DEG2RAD(yy * DSO_SEG_SIZE);
      trfRaDecToPointNoCorrect(&rd,&p);

      sector[y][x].p[0].w.x = p.w.x;
      sector[y][x].p[0].w.y = p.w.y;
      sector[y][x].p[0].w.z = p.w.z;

      rd.Ra = DEG2RAD((xx+1) * DSO_SEG_SIZE);
      trfRaDecToPointNoCorrect(&rd,&p);

      sector[y][x].p[1].w.x = p.w.x;
      sector[y][x].p[1].w.y = p.w.y;
      sector[y][x].p[1].w.z = p.w.z;

      rd.Ra = DEG2RAD((xx+1) * DSO_SEG_SIZE);
      rd.Dec = DEG2RAD((yy-1) * DSO_SEG_SIZE);
      trfRaDecToPointNoCorrect(&rd,&p);

      sector[y][x].p[2].w.x = p.w.x;
      sector[y][x].p[2].w.y = p.w.y;
      sector[y][x].p[2].w.z = p.w.z;

      rd.Ra = DEG2RAD(xx * DSO_SEG_SIZE);
      rd.Dec = DEG2RAD((yy-1) * DSO_SEG_SIZE);
      trfRaDecToPointNoCorrect(&rd,&p);

      sector[y][x].p[3].w.x = p.w.x;
      sector[y][x].p[3].w.y = p.w.y;
      sector[y][x].p[3].w.z = p.w.z;
    }
  }
}

/////////////////////////////////////////////////
void CDso::setPainter(CSkPainter *p, QImage *img)
/////////////////////////////////////////////////
{
  pPainter = p;
  pImg = img;
}

//////////////////////////////////////////
QStringList CDso::getNameList(dso_t *pDso)
//////////////////////////////////////////
{
  //return QStringList() << "ABCD1";
  return namesMap[pDso->nameOffs];
}

//////////////////////////////////
QString CDso::getName(dso_t *pDso)
//////////////////////////////////
{
  if (pDso->nameOffs == -1) return QString();
  return namesMap[pDso->nameOffs].at(0);
}

///////////////////////////////////////////
QString CDso::getName(dso_t *pDso, int idx)
///////////////////////////////////////////
{
  QStringList *list = &namesMap[pDso->nameOffs];
  if (idx >= list->count())
  {
    return QString();
  }

  return list->at(idx);
}

/////////////////////////////////////////////////
static QString removeSpacesLN(const QString text)
/////////////////////////////////////////////////
{
  QString result = text;

  //qDebug() << text;
  //Q_ASSERT(result.size() >= 2);
  if (result.isEmpty()) return "";

  QString tmp = (QString)result[0];

  for (int i = 1; i < result.size() - 1; i++)
  {
    if (result[i] == ' ' && !result[i - 1].isDigit() && result[i + 1].isDigit())
    {
      continue;
    }
    tmp += result[i];
  }

  tmp += result[result.size() - 1];

  return tmp;
}

/////////////////////////////////////////
QStringList CDso::getNameInt(dso_t *pDso)
/////////////////////////////////////////
{ 
  char *p = dsoNames + pDso->nameOffs;
  QString str = QString(p);

  //qDebug() << p;
  str = removeSpacesLN(str);  
  //qDebug() << str;

  if (str.isEmpty())
  {
    qDebug() << p << pDso->nameOffs << pDso->type;
    qDebug() << "empty??" << getStrRA(pDso->rd.Ra) << getStrDeg(pDso->rd.Dec) << pDso->mag << pDso->type << pDso->sx;
  }  

  return str.split("\t");
}

void CDso::applyNameFilter()
{
  bool param = g_skSet.map.dsoFilterType;

  if (g_skSet.map.dsoFilter.isEmpty())
  {
    param = false;
  }

  QStringList dsoFilterList = g_skSet.map.dsoFilter.remove(" ").split(";");

  for (qint32 i = 0; i < dsoHead.numDso; i++)
  {
    QString name = cDSO.getName(&dso[i]);

    dso[i].show = !param;

    foreach (const QString &filter, dsoFilterList)
    {
      if (filter.isEmpty())
      {
        continue;
      }

      QRegExp re(filter + "[\\s0-9]");
      re.setCaseSensitivity(Qt::CaseInsensitive);

      if (name.indexOf(re) == 0)
      {
        dso[i].show = param;//g_skSet.map.dsoFilterType;
        break;
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
int CDso::renderNebulaSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
/////////////////////////////////////////////////////////////////////////////
{
  int b = 0;

  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int sy = trfGetArcSecToPix(pDso->sy);
  int s = qMax(sx, sy);

  s = qMax(s, m_minSize);
  pPainter->drawRect(QRect(pt->sx - s, pt->sy - s, s * 2, s * 2));

  m_lastSize = s;

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    b = s;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_RECT, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


//////////////////////////////////////////////////////////////////////////////
int CDso::renderOpenClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
//////////////////////////////////////////////////////////////////////////////
{
  int b = 0;

  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int  s = qMax(sx, m_minSize);

  m_lastSize = s;

  pPainter->drawEllipse(QPoint(pt->sx, pt->sy), s, s);

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    b = s;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


//////////////////////////////////////////////////////////////////////////////
int CDso::renderGlobClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
//////////////////////////////////////////////////////////////////////////////
{
  int b = 0;

  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int  s = qMax(sx, m_minSize);

  m_lastSize = s;

  pPainter->drawEllipse(QPoint(pt->sx, pt->sy), s, s);
  pPainter->drawCross(pt->sx, pt->sy, s);

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    b = s;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


////////////////////////////////////////////////////////////////////////////////
int CDso::renderPlnNebulaSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
////////////////////////////////////////////////////////////////////////////////
{
  int b = 0;

  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int  s = qMax(sx, m_minSize);

  m_lastSize = s;

  pPainter->drawEllipse(QPoint(pt->sx, pt->sy), s, s);
  pPainter->drawEllipse(QPoint(pt->sx, pt->sy),(int)(s * 0.8f), (int)(s * 0.8f));

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    b = s;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
int CDso::renderGalaxySymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, mapView_t *mapView, bool addToList)
/////////////////////////////////////////////////////////////////////////////////////////////////
{  
  int sz = m_minSize - 1;
  pPainter->setPen(m_pen);    

  int sx = trfGetArcSecToPix(pDso->sx);
  int sy = trfGetArcSecToPix(pDso->sy);
  int s = qMax(sx, sy);

  if (s <= sz)
  {
    pPainter->drawEllipse(QPoint(pt->sx, pt->sy), sz * 2, sz);
    if (addToList)
    {
      addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, sz, (qint64)pDso, 0, pDso->DSO_MAG);
    }
    m_lastSize = s;
    return 0;
  }

  double ra = pDso->rd.Ra;
  double dec = pDso->rd.Dec;

  double ang = (float)trfGetAngleToNPole(ra, dec);

  if (pDso->pa != NO_DSO_PA)
  {
    if (mapView->flipX + mapView->flipY == 1)
      ang = (D2R(pDso->pa) + ang);
    else
      ang = -(D2R(pDso->pa) - ang);
  }

  pPainter->save();    
  pPainter->translate(pt->sx, pt->sy);
  pPainter->rotate(R2D(ang));
  pPainter->drawEllipse(QPoint(0, 0), sy, sx);
  pPainter->restore();

  int b = s;
  m_lastSize = s;

  if (g_showLabels)
  {
    float uy = sy * sin( - ang);
    float vy = sx * sin( - ang + MPI/2);
    int y = sqrt(uy*uy + vy*vy);

    g_labeling.addLabel(QPoint(pt->sx, pt->sy + y + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());

    b = y;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


////////////////////////////////////////////////////////////////////////////////
int CDso::renderGalaxyClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
////////////////////////////////////////////////////////////////////////////////
{
  bool b =false;

  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int  s = qMax(sx, m_minSize);

  m_lastSize = s;

  pPainter->drawEllipse(QPoint(pt->sx, pt->sy), s, s);

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    b = true;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


//////////////////////////////////////////////////////////////////////////////
int CDso::renderDsoStarSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
//////////////////////////////////////////////////////////////////////////////
{
  bool b = false;

  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int  s = qMax(sx, m_minSize);

  m_lastSize = s;

  pPainter->drawEllipse(QPoint(pt->sx, pt->sy), s, s);
  pPainter->drawHalfCross(pt->sx, pt->sy, s, s / 2);

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    b = true;
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return b;
}


////////////////////////////////////////////////////////////////////////////
int CDso::renderOtherSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList)
////////////////////////////////////////////////////////////////////////////
{  
  pPainter->setPen(m_pen);

  int sx = trfGetArcSecToPix(pDso->sx);
  int  s = qMax(sx, m_minSize);

  m_lastSize = s;

  pPainter->drawCross(pt->sx, pt->sy, s);  

  if (s > m_minSize && g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy + s + m_fntHeight), 0, getName(pDso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());    
  }
  if (addToList)
  {
    addMapObj(pDso->rd, pt->sx, pt->sy, MO_DSO, MO_CIRCLE, s, (qint64)pDso, 0, pDso->DSO_MAG);
  }

  return 0;
}

int CDso::lastRenderedSize()
{
  return m_lastSize;
}

///////////////////////////////////////////////////////////////////////////
void CDso::drawShape(QPainter *p, QImage *img, dso_t *dso, mapView_t *view, bool addToList)
///////////////////////////////////////////////////////////////////////////
{
  QRect  rc;
  QColor cols[3] = {g_skSet.map.dsoShapeColor[0],
                    g_skSet.map.dsoShapeColor[1],
                    g_skSet.map.dsoShapeColor[2]};

  if (tShapeList[dso->shape]->render(p, img, view, cols, rc))
  {
    if (g_showLabels)
    {
      g_labeling.addLabel(QPoint(rc.center().x(), rc.bottom() + m_fntHeight), 0, getName(dso), FONT_DSO, SL_AL_CENTER, SL_AL_FIXED, pPainter->opacity());
    }
    if (addToList)
    {
      addMapObj(dso->rd, rc.center().x(), rc.center().y(), MO_DSO, MO_RECT, rc.width() / 2, (qint64)dso, 0, dso->DSO_MAG);
    }
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////
int CDso::renderObj(SKPOINT *pt, dso_t *pDso, mapView_t *mapView, bool addToList, double opacity)
//////////////////////////////////////////////////////////////////////////////////////////////////
{
  if (pDso->shape != NO_DSO_SHAPE)
  {
    if (g_showDSOShapes)
    {
      drawShape(pPainter, pImg, pDso, mapView, addToList);
      return 0;
    }
  }

  pPainter->setOpacity(opacity);

  QFontMetrics fmt(setFonts[FONT_DSO]);
  m_fntHeight = fmt.height();

  switch (pDso->type)
  {
    case DSOT_NEBULA:
    case DSOT_SUPER_REM:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_NEBULA]),
                   g_skSet.map.dsoWidth[DSO_COL_NEBULA],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_NEBULA]);
      break;

    case DSOT_DARK_NEB:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_DARK_NEB]),
                   g_skSet.map.dsoWidth[DSO_COL_DARK_NEB],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_DARK_NEB]);
      break;

    case DSOT_BRIGHT_NEB:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_BRIGHT_NEB]),
                   g_skSet.map.dsoWidth[DSO_COL_BRIGHT_NEB],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_BRIGHT_NEB]);
      break;

    case DSOT_OPEN_CLS_NEB:
    case DSOT_OPEN_CLUSTER:
    case DSOT_GALAXY_CLD:
    case DSOT_MILKY_SC:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_OPEN_CLS]),
                   g_skSet.map.dsoWidth[DSO_COL_OPEN_CLS],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_OPEN_CLS]);
      break;

    case DSOT_GLOB_CLUSTER:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_GLOB_CLS]),
                   g_skSet.map.dsoWidth[DSO_COL_GLOB_CLS],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_GLOB_CLS]);
      break;

    case DSOT_PLN_NEBULA:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_PLN_NEB]),
                   g_skSet.map.dsoWidth[DSO_COL_PLN_NEB],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_PLN_NEB]);
      break;

    case DSOT_GALAXY:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_GALAXY]),
                   g_skSet.map.dsoWidth[DSO_COL_GALAXY],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_GALAXY]);
      break;

    case DSOT_GAL_CLUSTER:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_GAL_CLS]),
                   g_skSet.map.dsoWidth[DSO_COL_GAL_CLS],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_GAL_CLS]);
      break;

    case DSOT_STAR:
    case DSOT_STARS:
    case DSOT_ASTERISM:
    case DSOT_QUASAR:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_STAR]),
                   g_skSet.map.dsoWidth[DSO_COL_STAR],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_STAR]);
      break;

    case DSOT_OTHER:
    case DSOT_UNKNOWN:
      m_pen = QPen(QColor(g_skSet.map.dsoColors[DSO_COL_OTHER]),
                   g_skSet.map.dsoWidth[DSO_COL_OTHER],
                  (Qt::PenStyle)g_skSet.map.dsoStyle[DSO_COL_OTHER]);
      break;
  }

  pPainter->setBrush(Qt::NoBrush);

  int b = 0;

  switch (pDso->type)
  {
    case DSOT_NEBULA:
    case DSOT_DARK_NEB:
    case DSOT_BRIGHT_NEB:
    case DSOT_SUPER_REM:
      b = renderNebulaSymbol(pt, pDso, pPainter, addToList);
      break;

    case DSOT_OPEN_CLS_NEB:
    case DSOT_OPEN_CLUSTER:
    case DSOT_MILKY_SC:
    case DSOT_GALAXY_CLD:
      b = renderOpenClsSymbol(pt, pDso, pPainter, addToList);
      break;

    case DSOT_GLOB_CLUSTER:
      b = renderGlobClsSymbol(pt, pDso, pPainter, addToList);
      break;

    case DSOT_PLN_NEBULA:
    case DSOT_NGC_DUPP:
      b = renderPlnNebulaSymbol(pt, pDso, pPainter, addToList);
      break;

    case DSOT_GALAXY:
      b = renderGalaxySymbol(pt, pDso, pPainter, mapView, addToList);
      break;

    case DSOT_GAL_CLUSTER:
      b = renderGalaxyClsSymbol(pt, pDso, pPainter, addToList);
      break;

    case DSOT_STAR:
    case DSOT_STARS:
    case DSOT_ASTERISM:
    case DSOT_QUASAR:
      b = renderDsoStarSymbol(pt, pDso, pPainter, addToList);
      break;

    case DSOT_OTHER:
    case DSOT_UNKNOWN:
      b = renderOtherSymbol(pt, pDso, pPainter, addToList);
      break;
  }  

  pPainter->setOpacity(1);

  return b;
}



