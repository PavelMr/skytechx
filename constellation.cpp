#include "constellation.h"
#include "jd.h"
#include "precess.h"
#include "skcore.h"
#include "setting.h"

#define NBNDRIES 357

typedef struct
{
  radec_t rd;
    short p;
} constBndLines_t;

#pragma pack(2)

typedef struct
{
  double Ra1, Ra2;
  double Dec;
     int index;
} constBnd_t;

#pragma pack()

QList <constelLine_t> tConstLines;

static qint32 numConstelNBnd = 0;
static constBndLines_t *constelNBnd;

static bool    nonLatinLoaded = false;
static QString constelNonLatinNames[88];
static QString constel2nd[88];
static QString constelLongNames[88];
static QString constelShort[88];
static radec_t constelRD[88];

static constBnd_t constBnd[360];

extern bool g_showLabels;
extern int  dev_const_type;
extern int  dev_const_sel;


//////////////////////////////////////////////
QList <constelLine_t> *constGetLinesList(void)
//////////////////////////////////////////////
{
  return(&tConstLines);
}

/////////////////////////////////////////////////////////
void getConstPosition(int index, double &ra, double &dec)
/////////////////////////////////////////////////////////
{
  Q_ASSERT(index >= 0 && index < 88);

  ra = constelRD[index].Ra;
  dec = constelRD[index].Dec;
}

/////////////////////////////////////////////////////////////////////////////
bool constFind(QString name, double &ra, double &dec, double &fov, double jd)
/////////////////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < 88; i++)
  {
    if ((constelShort[i].compare(name, Qt::CaseInsensitive) == 0) ||
        (constelLongNames[i].compare(name, Qt::CaseInsensitive) == 0))
    {
      ra = constelRD[i].Ra;
      dec = constelRD[i].Dec;
      precess(&ra, &dec, JD2000, jd);
      fov = DEG2RAD(50);
      return(true);
    }
  }
  return(false);
}

/////////////////////////////////////////
// 0 - desc, 1- long 2 - 2nd long
QString constGetName(int index, int type, bool addNonLatin)
/////////////////////////////////////////
{
  if (addNonLatin && nonLatinLoaded)
  {
    switch (type)
    {
      case 0:
        return(constelShort[index] + " - " + constelNonLatinNames[index]);
      case 1:
        return(constelLongNames[index] + " - " + constelNonLatinNames[index]);
      case 2:
        return(constel2nd[index] + " - " + constelNonLatinNames[index]);
    }
  }
  else
  {
    switch (type)
    {
      case 0:
        return(constelShort[index]);
      case 1:
        return(constelLongNames[index]);
      case 2:
        return(constel2nd[index]);
    }
  }

  return(0);
}


/////////////////////////////////////////////////////////
int constWhatConstel(double Ra, double Dec, double epoch)
/////////////////////////////////////////////////////////
{
  // Besselian epoch 1875.0
  precess(&Ra, &Dec, epoch, 2405889.25855);

  for (int a = 0; a < NBNDRIES ;a++)
  {
    if (constBnd[a].Dec > Dec) continue;
    if (constBnd[a].Ra2 <= Ra) continue;
    if (constBnd[a].Ra1 > Ra) continue;

    if (Ra >= constBnd[a].Ra1 &&  Ra < constBnd[a].Ra2 && constBnd[a].Dec <= Dec)
          return(constBnd[a].index);
     else if (constBnd[a].Ra2 <  Ra) continue;
     else return(0);

     break;
  }
  return(0);
}


/////////////////////////////////////
static void constLoadBoundaries(void)
/////////////////////////////////////
{
  // read constelation boundaries ra/dec
  SkFile f("data/constellation/constel.def");
  if (f.open(SkFile::ReadOnly))
  {
    int tmp;
    f.read((char *)&tmp, 4);
    for (int i = 0; i < NBNDRIES; i++)
    {
      f.read((char *)&constBnd[i], sizeof(constBnd_t));
    }
    f.close();
  }

}

////////////////////////////////
static void constLoadNames(void)
////////////////////////////////
{
  // read constelation names
  SkFile f("data/constellation/constel.nam");
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    int         i = 0;
    QString     str;
    QStringList list;
    // TODO: upravit lepe pozice ra/dec nazvu
    do
    {
      str = f.readLine();
      if (str.isEmpty())
        break;
      list = str.split("|");
      if (list.count() != 5)
      { // TODO: neco udelat
        qDebug("constLoadNames fail!");
        QApplication::exit(1);
      }

      if (i >= 88)
      { // TODO: neco udelat
        qDebug("constLoadNames fail! (too many constellations)");
        QApplication::exit(1);
      }

      constelLongNames[i] = list[0].simplified();
      constel2nd[i] = list[1].simplified();
      constelShort[i] = list[2].simplified();

      QStringList ra = list[3].simplified().split(" ");

      constelRD[i].Ra = HMS2RAD(ra[0].toInt(), ra[1].toInt(), 0);
      constelRD[i].Dec = DEG2RAD(list[4].simplified().toDouble());
      i++;
    } while(1);

    f.close();
  }
}

void loadConstelNonLatinNames(const QString &name)
{
  nonLatinLoaded = false;

  if (name.isEmpty())
  {
    return;
  }

  SkFile f(name);
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    int         i = 0;
    QString     str;

    do
    {
      str = f.readLine();
      if (str.isEmpty())
        break;

      if (i >= 88)
      {
        qDebug("loadConstelNonLatinNames fail! (too many constellations)");
        msgBoxError(NULL, "Invalid constellation count!!!");
        QApplication::exit(1);
      }

      constelNonLatinNames[i] = str.simplified();

      i++;
    } while(1);

    f.close();
    nonLatinLoaded = true;
  }
}

/////////////////////////////////
void constLinesLoad(QString name)
/////////////////////////////////
{
  SkFile f;

  tConstLines.clear();

  f.setFileName(name);
  if (f.open(SkFile::ReadOnly))
  {
    qint32 cnt;
    f.read((char *)&cnt, sizeof(qint32));

    for (int i = 0; i < cnt; i++)
    {
      constelLine_t l;

      f.read((char *)&l.pt.Ra, sizeof(double));
      f.read((char *)&l.pt.Dec, sizeof(double));
      f.read((char *)&l.cmd, sizeof(qint32));

      tConstLines.append(l);
    }
    f.close();
  }
}


/////////////////////////////////////
static void constLoadBoundLines(void)
/////////////////////////////////////
{
  // read constellation boundaries
  SkFile f("data\\constellation\\constel.bnd");
  if (f.open(SkFile::ReadOnly))
  {
    f.read((char *)&numConstelNBnd, sizeof(qint32));
    constelNBnd = (constBndLines_t *)malloc(numConstelNBnd * sizeof(constBndLines_t));
    for (int i = 0; i < numConstelNBnd; i++)
    {
      f.read((char *)&constelNBnd[i].rd.Ra, sizeof(double));
      f.read((char *)&constelNBnd[i].rd.Dec, sizeof(double));
      f.read((char *)&constelNBnd[i].p, sizeof(short));
    }
    f.close();
  }
}


////////////////////
void constLoad(void)
////////////////////
{
  qDebug() << "pack" << sizeof(constelLine_t);
  constLinesLoad(g_skSet.map.constellation.linesFile);
  constLoadBoundLines();
  constLoadNames();
  constLoadBoundaries();
}


/////////////////////////////////////////////////////
static double getAnSepRa(double r1, double r2, bool *pbWrap)
/////////////////////////////////////////////////////
{
  double v;

  v = fabs(r1 - r2);
  if (v > MPI)
  {
    v -= MPI2;
    v = fabs(v);
    *pbWrap = true;
  } else *pbWrap = false;

  return(v);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
static void drawInterpolatedLineRD(int c, radec_t *rd1, radec_t *rd2, QPainter *pPainter, SKMATRIX *prec)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  bool wrap;

  if (rd1->Ra > rd2->Ra)
  {
    qSwap(rd1, rd2);
  }

  getAnSepRa(rd2->Ra, rd1->Ra, &wrap);

  if (wrap)
    rd1->Ra += R360;

  double dx = (rd2->Ra - rd1->Ra) / (double)(c - 1);
  double dy = (rd2->Dec - rd1->Dec) / (double)(c - 1);
  radec_t rd = {rd1->Ra, rd1->Dec};

  SKPOINT pt1;
  SKPOINT pt2;

  trfRaDecToPointNoCorrect(&rd, &pt1, prec);

  for (int i = 0; i < c - 1; i++)
  {
    rd.Ra += dx;
    rd.Dec += dy;
    trfRaDecToPointNoCorrect(&rd, &pt2, prec);

    //trfRaDecToPointNoCorrect(&rd, &pt1, prec);
    //rd.Ra += dx;
    //rd.Dec += dy;
    //trfRaDecToPointNoCorrect(&rd, &pt2, prec);

    if (trfProjectLine(&pt1, &pt2))
    {
      pPainter->drawLine(pt1.sx, pt1.sy, pt2.sx, pt2.sy);
    }
    pt1 = pt2;
  }
}


//////////////////////////////////////////////////////////////////
void constRenderConstellationNames(CSkPainter *p, mapView_t *view)
//////////////////////////////////////////////////////////////////
{
  if (!g_showLabels)
    return;

  int w = p->device()->width();
  int h = p->device()->height();

  setSetFontColor(FONT_CONST, p);
  setSetFont(FONT_CONST, p);

  if (view->fov <= D2R(35))
  { // draw to corner
    radec_t corner[4];
    int     con;
    int     m = 5;

    trfConvScrPtToXY(m, m, corner[0].Ra, corner[0].Dec);
    trfConvScrPtToXY(w - m, m, corner[1].Ra, corner[1].Dec);
    trfConvScrPtToXY(w - m, h - m, corner[2].Ra, corner[2].Dec);
    trfConvScrPtToXY(m, h - m, corner[3].Ra, corner[3].Dec);

    QList <int> tList;

    for (int i = 0; i < 4; i++)
    {
      con = constWhatConstel(corner[i].Ra, corner[i].Dec, JD2000);

      if (tList.contains(con))
        continue;
      tList.append(con);

      QString text =  nonLatinLoaded ? constelNonLatinNames[con] : constelShort[con];

      switch (i)
      {
        case 0:
          p->drawTextLR(m, m, text);
          break;

        case 1:
          p->drawTextLL(w - m, m, text);
          break;

        case 2:
          p->drawTextUL(w - m, h - m, text);
          break;

        case 3:
          p->drawTextUR(m, h - m, text);
          break;
      }
    }
    return;
  }

  for (int i = 0; i < 88; i++)
  {
    { // draw normal
      SKPOINT pt;

      trfRaDecToPointNoCorrect(&constelRD[i], &pt);
      if (trfProjectPoint(&pt))
      {
        p->drawCText(pt.sx, pt.sy, nonLatinLoaded ? constelNonLatinNames[i] : constelShort[i]);
      }
    }
  }
}


//////////////////////////////////////////////////////////////
void constRenderConstellationBnd(QPainter *p, mapView_t *view)
//////////////////////////////////////////////////////////////
{
  int       inter;
  double    df;
  SKPOINT   p1, p2;
  SKMATRIX  prec;

  QPen pen(QColor(g_skSet.map.constellation.bnd.color),
           g_skSet.map.constellation.bnd.width,
           (Qt::PenStyle)g_skSet.map.constellation.bnd.style);;

  p->setPen(pen);

  precessMatrix(2405889.25855, JD2000, &prec);

  df = RAD2DEG(view->fov);
  if (df < 15)
    inter = 5;
  else
    inter = 1;

  constBndLines_t *pCon;
  constBndLines_t *pCon2;

  for (int a = 0; a < numConstelNBnd - 1; a++)
  {
    pCon  = &constelNBnd[a];
    pCon2 = &constelNBnd[a + 1];

    if (pCon->p)
    {
      if (inter > 1 && fabs(pCon->rd.Dec - pCon2->rd.Dec) < DEG2RAD(0.25))
      {
        drawInterpolatedLineRD(inter, &pCon2->rd, &pCon->rd, p, &prec);
      }
      else
      {
        trfRaDecToPointNoCorrect(&pCon->rd, &p1, &prec);
        trfRaDecToPointNoCorrect(&pCon2->rd, &p2, &prec);

        if (trfProjectLine(&p1, &p2))
         p->drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////
void constRenderConstelationLines(QPainter *p, const mapView_t *view)
/////////////////////////////////////////////////////////////////////
{
  constelLine_t l;
  SKPOINT pt1;
  SKPOINT pt2;
  radec_t from;
  radec_t to;
  QPen    pn1(QColor(g_skSet.map.constellation.main.color),
              g_skSet.map.constellation.main.width,
              (Qt::PenStyle)g_skSet.map.constellation.main.style);
  QPen    pn2(QColor(g_skSet.map.constellation.sec.color),
              g_skSet.map.constellation.sec.width,
              (Qt::PenStyle)g_skSet.map.constellation.sec.style);

  foreach (l, tConstLines)
  {
    switch (l.cmd)
    {
      case 0: // data from
        from = l.pt;
        break;

      case 1: // 1-main line to
        to = l.pt;
        p->setPen(pn1);
        break;

      case 2: // 2-sec. line to
        to = l.pt;
        p->setPen(pn2);
        break;
    }

    if (l.cmd > 0)
    {
      trfRaDecToPointNoCorrect(&from, &pt1);
      trfRaDecToPointNoCorrect(&to, &pt2);

      if (trfProjectLine(&pt1, &pt2))
      {
        double x1 = pt1.sx;
        double x2 = pt2.sx;
        double y1 = pt1.sy;
        double y2 = pt2.sy;

        double dx = x2 - x1;
        double dy = y2 - y1;
        double dist = sqrt(dx*dx + dy*dy);

        dx /= dist;
        dy /= dist;

        double d;

        if (view->fov > D2R(50))
          d = 10;
        else
        if (view->fov > D2R(20))
          d = 15;
        else
        if (view->fov > D2R(5))
          d = 20;
        else
          d = 25;

        double f1 = d;
        double f2 = dist - d;

        double x3 = x1 + f1 * dx;
        double y3 = y1 + f1 * dy;
        double x4 = x1 + f2 * dx;
        double y4 = y1 + f2 * dy;

        p->drawLine(x3, y3, x4, y4);
      }
      from = to;
    }
  }
}


////////////////////////////////////////////////////////////////////
void constRenderConstelationLines2Edit(QPainter *p, mapView_t *view)
////////////////////////////////////////////////////////////////////
{
  constelLine_t *l;
  SKPOINT pt1;
  SKPOINT pt2;
  radec_t from;
  radec_t to;
  QPen    pn1(QColor(200, 0, 0), 2, Qt::SolidLine);
  QPen    pn3(QColor(255, 0, 0), 2, Qt::SolidLine);
  QPen    pn2(QColor(200, 0, 0), 2, Qt::DotLine);
  QPen    pns(QColor(255, 255, 0), 3, Qt::SolidLine);

  p->setBrush(Qt::NoBrush);

  // 0-data, 1-main line to, 2-sec. line to

  for (int i = 0; i < tConstLines.count(); i++)
  {
    l = &tConstLines[i];

    switch (l->cmd)
    {
      case 0: // data from
        from = l->pt;
        if (dev_const_sel == i)
          p->setPen(pns);
        else
          p->setPen(pn3);
        break;

      case 1:
        to = l->pt;
        p->setPen(pn1);
        break;

      case 2:
        to = l->pt;
        p->setPen(pn2);
        break;
    }

    if (l->cmd > 0)
    {
      trfRaDecToPointCorrectFromTo(&from, &pt1, JD2000, view->jd);
      trfRaDecToPointCorrectFromTo(&to, &pt2, JD2000, view->jd);

      if (trfProjectLine(&pt1, &pt2))
      {
        p->drawLine(pt1.sx, pt1.sy, pt2.sx, pt2.sy);
        if (dev_const_sel == i)
          p->setPen(pns);
        else
          p->setPen(pn3);
        p->drawEllipse(QPoint(pt2.sx, pt2.sy), 5, 5);
      }

      from = to;
    }
    else
    {
      trfRaDecToPointCorrectFromTo(&from, &pt1, JD2000, view->jd);
      if (trfProjectPoint(&pt1))
      {
        p->drawEllipse(QPoint(pt1.sx, pt1.sy), 5, 5);
      }
    }
  }
}


