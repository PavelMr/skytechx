#include "cdrawing.h"
#include "cmapview.h"
#include "setting.h"
#include "skcore.h"

#define ROT_MARGIN        10

CDrawing g_cDrawing; // TODO: nepouzivat globalne promene s QObject kvuli tr();

// FIXME: obcas se v dialogu objevuji prazdne radky!!!!!

void setHelpText(QString text);

static QList <drawing_t> m_tList;

drawing_t *getDrawing(int index)
{
  if (index >= m_tList.count())
    return NULL;

  return &m_tList[index];
}

void deleteDrawing(drawing_t *draw)
{
  for (int i = 0; i < m_tList.count(); i++)
  {
    if (&m_tList[i] == draw)
    {
      m_tList.removeAt(i);
      break;
    }
  }
}

//////////////////////
void drawingSave(void)
//////////////////////
{
  SkFile f("data/drawing/drawing.dat");
  QDataStream s(&f);

  if (f.open(SkFile::WriteOnly))
  {
    int count = m_tList.count();    

    s << count;

    for (int i = 0; i < count; i++)
    {
      drawing_t t = m_tList.at(i);      

      s << t.type;
      s << t.show;
      s << t.onScr;
      s << t.angle;
      s << t.rect;
      s << t.rd.Ra;
      s << t.rd.Dec;

      s << t.frmField_t.x;
      s << t.frmField_t.y;
      s << t.frmField_t.text;

      s << t.telescope_t.rad;
      s << t.telescope_t.name;

      s << t.text_t.text;
      s << t.text_t.align;
      s << t.text_t.bRect;
      s << t.text_t.font;
    }
  }
}


//////////////////////
void drawingLoad(void)
//////////////////////
{
  SkFile f("data/drawing/drawing.dat");
  QDataStream s(&f);

  m_tList.clear();

  if (f.open(SkFile::ReadOnly))
  {
    int count;

    s >> count;

    for (int i = 0; i < count; i++)
    {
      drawing_t t;

      s >> t.type;
      s >> t.show;
      s >> t.onScr;
      s >> t.angle;
      s >> t.rect;
      s >> t.rd.Ra;
      s >> t.rd.Dec;

      s >> t.frmField_t.x;
      s >> t.frmField_t.y;
      s >> t.frmField_t.text;

      s >> t.telescope_t.rad;
      s >> t.telescope_t.name;

      s >> t.text_t.text;
      s >> t.text_t.align;
      s >> t.text_t.bRect;
      s >> t.text_t.font;      

      m_tList.append(t);
    }
  }
}

CDrawing::CDrawing(QObject *parent) :
  QObject(parent)
{
  m_drawing.type = DT_NONE;

  txMove = QObject::tr("Move object by mouse.\n");
  txRotate = QObject::tr("Rotate object by an edge.\n");
  txDone = QObject::tr("ENTER : Done\nESC : Cancel\n");
}

///////////////////////////////////////
void CDrawing::setView(mapView_t *view)
///////////////////////////////////////
{
  m_view = *view;
  m_frustum = trfGetFrustum();
}

////////////////////////////////////////
void CDrawing::setEditedPos(radec_t *rd)
////////////////////////////////////////
{
  m_drawing.rd = *rd;
}

////////////////////////////////////////
void CDrawing::getEditedPos(radec_t *rd)
////////////////////////////////////////
{
  *rd = m_drawing.rd;
}

////////////////////////////////////////////////////////////////////
void CDrawing::insertTelescope(radec_t *rd, float rad, QString text)
////////////////////////////////////////////////////////////////////
{
  m_drawing.type = DT_TELESCOPE;
  m_drawing.rd = *rd;
  m_drawing.telescope_t.name = text;
  m_drawing.telescope_t.rad = rad;

  setHelpText(txMove + txDone);

  emit sigChange(true, m_tList.count() == 0);
}

////////////////////////////////////////
void CDrawing::insertTelrad(radec_t *rd)
////////////////////////////////////////
{
  m_drawing.type = DT_TELRAD;
  m_drawing.rd = *rd;

  setHelpText(txMove + txDone);

  emit sigChange(true, m_tList.count() == 0);
}

////////////////////////////////////////////////////////////////////////////
void CDrawing::insertFrmField(radec_t *rd, double x, double y, QString name)
////////////////////////////////////////////////////////////////////////////
{
  m_drawing.type = DT_FRM_FIELD;
  m_drawing.rd = *rd;
  m_drawing.frmField_t.text = name;
  m_drawing.frmField_t.x = x;
  m_drawing.frmField_t.y = y;
  m_drawing.angle = 0;

  setHelpText(txMove + txRotate + txDone);

  emit sigChange(true, m_tList.count() == 0);
}

////////////////////////////////////////////////////////////////////////////////////////
void CDrawing::insertText(radec_t *rd, QString name, QFont *font, int align, bool bRect)
////////////////////////////////////////////////////////////////////////////////////////
{
  m_drawing.type = DT_TEXT;
  m_drawing.rd = *rd;
  m_drawing.text_t.text = name;
  m_drawing.text_t.font = *font;
  m_drawing.text_t.align = align;
  m_drawing.text_t.bRect = bRect;

  setHelpText(txMove + txDone);

  emit sigChange(true, m_tList.count() == 0);
}

//////////////////////////////////////////////
void CDrawing::drawEditedObject(CSkPainter *p)
//////////////////////////////////////////////
{
  QPoint out;

  switch (m_drawing.type)
  {
    case DT_NONE:
      break;

    case DT_TELESCOPE:
      drawCircle(out, p, &m_drawing.rd,
                          m_drawing.telescope_t.rad,
                          m_drawing.telescope_t.name, true);
      break;

    case DT_TELRAD:
      drawTelrad(out, p, &m_drawing.rd, true);
      break;

    case DT_TEXT:
      drawText(out, p, &m_drawing, true);
      break;

    case DT_FRM_FIELD:
      drawFrmField(out, p, &m_drawing, true);
      break;
  }
}

/////////////////////////////////////////
void CDrawing::drawObjects(CSkPainter *p)
/////////////////////////////////////////
{
  for (int i = 0; i < m_tList.count(); i++)
  {
    QPoint out;

    if (!m_tList[i].show)
    {
      continue;
    }

    switch (m_tList[i].type)
    {
      case DT_NONE:
        break;

      case DT_TELESCOPE:
        drawCircle(out, p, &m_tList[i].rd,
                            m_tList[i].telescope_t.rad,
                            m_tList[i].telescope_t.name, false);
        break;

      case DT_TELRAD:
        drawTelrad(out, p, &m_tList[i].rd, false);
        break;

      case DT_FRM_FIELD:
        drawFrmField(out, p, &m_tList[i], false);
        break;
    }
  }
}

//////////////////////////////////////////////////////////
int CDrawing::editObject(QPoint pos, QPoint delta, int op)
//////////////////////////////////////////////////////////
{
  if (m_drawing.type == DT_NONE)
    return(DTO_NONE);

  if (!m_drawing.onScr)
    return(DTO_NONE);

  if (op == DTO_NONE && !m_drawing.rect.contains(pos))
    return(DTO_NONE);

  QRect moveRect = m_drawing.rect;

  if (op == DTO_NONE)
  { // check operation
    moveRect.adjust(ROT_MARGIN, ROT_MARGIN, -ROT_MARGIN, -ROT_MARGIN);

    if (!moveRect.contains(pos) && m_drawing.type == DT_FRM_FIELD)
      return(DTO_ROTATE);

    return(DTO_MOVE);
  }

  if (op == DTO_MOVE)
  {
    double ra, dec;
    double ra2, dec2;

    //TODO: funguje to divne u polu

    trfConvScrPtToXY(pos.x(), pos.y(), ra, dec);
    trfConvScrPtToXY(pos.x() + delta.x(), pos.y() + delta.y(), ra2, dec2);

    precess(&ra, &dec, pcMapView->m_mapView.jd, JD2000);
    precess(&ra2, &dec2, pcMapView->m_mapView.jd, JD2000);

    m_drawing.rd.Ra -= ra2 - ra;
    m_drawing.rd.Dec -= dec2 - dec;

    return(DTO_MOVE);
  }
  else
  if (op == DTO_ROTATE)
  {
    m_drawing.angle -= delta.x() * 0.25;
    return(DTO_ROTATE);
  }

  return(op);
}


/////////////////////
void CDrawing::done()
/////////////////////
{
  if (m_drawing.type == DT_NONE)
  {
    return;
  }

  m_drawing.show = true;
  m_tList.append(m_drawing);
  m_drawing.type = DT_NONE;
  setHelpText("");

  emit sigChange(false, m_tList.count() == 0);
}

///////////////////////
void CDrawing::cancel()
///////////////////////
{
  m_drawing.type = DT_NONE;
  setHelpText("");

  emit sigChange(false, m_tList.count() == 0);
}

/////////////////////
int CDrawing::count()
/////////////////////
{
  return(m_tList.count());
}

/////////////////////////
void CDrawing::clearAll()
/////////////////////////
{
  m_tList.clear();

  emit sigChange(false, m_tList.count() == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int CDrawing::drawCircle(QPoint &ptOut, CSkPainter *p, radec_t *rd, float rad, QString text, bool bEdited)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  SKPOINT pt;
  double  r2;
  int     r = 0;

  if (bEdited)
  {
    m_drawing.onScr = false;
  }

  if (rad > 0)
    r2 = rad;
  else
    r2 = 0;

  trfRaDecToPointNoCorrect(rd, &pt);
  if (SKPLANECheckFrustumToSphere(m_frustum, &pt.w, r2))
  {
    if (rad > 0)
      r = (int)(p->device()->width() / RAD2DEG(m_view.fov) * RAD2DEG(rad) / 2.0);
    else
      r = fabs(rad);

    if (r < 5)
      r = 5;

    p->setPen(g_skSet.map.drawing.color);
    p->setBrush(Qt::NoBrush);

    trfProjectPointNoCheck(&pt);
    p->drawEllipse(QPoint(pt.sx, pt.sy), r, r);

    p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
    p->drawHalfCross(pt.sx, pt.sy, r, r * 0.2);
    p->drawCross(pt.sx, pt.sy, r * 0.1);

    setSetFontColor(FONT_DRAWING, p);
    setSetFont(FONT_DRAWING, p);
    p->drawText(QRect(pt.sx - 10000, pt.sy - 10000 + 5 + r + p->fontMetrics().height(), 20000, 20000), Qt::AlignCenter, text);

    if (bEdited)
    {
      p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
      p->drawRect(pt.sx - r, pt.sy - r, r * 2, r * 2);

      m_drawing.onScr = true;
      m_drawing.rect = QRect(pt.sx - r, pt.sy - r, r * 2, r * 2);
    }

    ptOut.setX(pt.sx);
    ptOut.setY(pt.sy);
  }

  return(r);
}

/////////////////////////////////////////////////////////////////////////////////
int CDrawing::drawTelrad(QPoint &ptOut, CSkPainter *p, radec_t *rd, bool bEdited)
/////////////////////////////////////////////////////////////////////////////////
{
  SKPOINT pt;
  int     r1 = 0;
  int     r2 = 0;
  int     r3 = 0;
  double  radius1 = D2R(4.0);
  double  radius2 = D2R(2.0);
  double  radius3 = D2R(0.5);
  double  cs = sin(D2R(45)) * 1.05;

  if (bEdited)
  {
    m_drawing.onScr = false;
  }

  trfRaDecToPointNoCorrect(rd, &pt);
  if (SKPLANECheckFrustumToSphere(m_frustum, &pt.w, radius1))
  {
    r1 = (int)(p->device()->width() / RAD2DEG(m_view.fov) * RAD2DEG(radius1) / 2.0);
    r2 = (int)(p->device()->width() / RAD2DEG(m_view.fov) * RAD2DEG(radius2) / 2.0);
    r3 = (int)(p->device()->width() / RAD2DEG(m_view.fov) * RAD2DEG(radius3) / 2.0);

    p->setPen(QPen(QColor(g_skSet.map.drawing.color), 2));
    p->setBrush(Qt::NoBrush);

    trfProjectPointNoCheck(&pt);
    p->drawEllipse(QPoint(pt.sx, pt.sy), r1, r1);
    p->drawEllipse(QPoint(pt.sx, pt.sy), r2, r2);
    p->drawEllipse(QPoint(pt.sx, pt.sy), r3, r3);

    p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
    p->drawCross(pt.sx, pt.sy, r1);

    if (r1 > 100)
    {
      setSetFontColor(FONT_DRAWING, p);
      setSetFont(FONT_DRAWING, p);

      p->drawTextLR(pt.sx + r1 * cs, pt.sy + r1 * cs, "4°");
      p->drawTextLR(pt.sx + r2 * cs, pt.sy + r2 * cs, "2°");
      p->drawTextLR(pt.sx + r3 * cs, pt.sy + r3 * cs, "0.5°");
    }

    if (bEdited)
    {
      p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
      p->drawRect(pt.sx - r1, pt.sy - r1, r1 * 2, r1 * 2);

      m_drawing.onScr = true;
      m_drawing.rect = QRect(pt.sx - r1, pt.sy - r1, r1 * 2, r1 * 2);
    }

    ptOut.setX(pt.sx);
    ptOut.setY(pt.sy);
  }

  return(r1);
}

//////////////////////////////////////////////////////////////////////////////////
// TODO: dodelat to
int CDrawing::drawText(QPoint &ptOut, CSkPainter *p, drawing_t *drw, bool bEdited)
//////////////////////////////////////////////////////////////////////////////////
{
  SKPOINT pt;
  int     w, h;
  QFontMetrics mt(drw->text_t.font);

  if (bEdited)
  {
    m_drawing.onScr = false;
  }

  w = mt.width(drw->text_t.text);
  h = mt.height();

  trfRaDecToPointNoCorrect(&drw->rd, &pt);
  if (SKPLANECheckFrustumToSphere(m_frustum, &pt.w, 0))
  //if (trfPointOnScr(pt.sx, pt.sy))
  {
    p->setPen(QPen(QColor(g_skSet.map.drawing.color), 2));
    p->setBrush(Qt::NoBrush);

    trfProjectPointNoCheck(&pt);
    //p->drawEllipse(QPoint(pt.sx, pt.sy), r, r);

    //p->setPen(QPen(QColor(255, 255, 255), 1, Qt::DotLine));
    //p->drawHalfCross(pt.sx, pt.sy, r, r * 0.2);
    //p->drawCross(pt.sx, pt.sy, r * 0.1);
    //p->drawText(QRect(pt.sx - 10000, pt.sy - 10000 + 5 + r + p->fontMetrics().height(), 20000, 20000), Qt::AlignCenter, text);

    setSetFontColor(FONT_DRAWING, p);
    setSetFont(FONT_DRAWING, p);
    p->drawText(QRect(pt.sx, pt.sy, w, h), Qt::AlignCenter, drw->text_t.text);

    if (bEdited)
    {
      p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
      p->drawRect(pt.sx, pt.sy, w, h);

      m_drawing.onScr = true;
      m_drawing.rect = QRect(pt.sx, pt.sy, w, h);
    }
    else
    {
      if (drw->text_t.bRect)
      {
        p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1));
        p->drawRect(pt.sx, pt.sy, w, h);
      }
    }

    ptOut.setX(pt.sx);
    ptOut.setY(pt.sy);
  }

  return(w);
}

void CDrawing::calcFrmField(CSkPainter *p, drawing_t *drw)
{
  radec_t rd[4];
  SKMATRIX matRot;
  SKMATRIX matRa;
  SKMATRIX matDec;
  SKMATRIX mat;

  rd[0].Ra =  -0.5 * drw->frmField_t.x;
  rd[0].Dec = -0.5 * drw->frmField_t.y;

  rd[1].Ra =   0.5 * drw->frmField_t.x;
  rd[1].Dec = -0.5 * drw->frmField_t.y;

  rd[2].Ra =  0.5 * drw->frmField_t.x;
  rd[2].Dec = 0.5 * drw->frmField_t.y;

  rd[3].Ra = -0.5 * drw->frmField_t.x;
  rd[3].Dec = 0.5 * drw->frmField_t.y;

  SKMATRIXRotateZ(&matRot, D2R(drw->angle));
  SKMATRIXRotateY(&matRa, -drw->rd.Ra);
  SKMATRIXRotateX(&matDec, drw->rd.Dec);

  mat = matRot * matRa * matDec;

  for (int i = 0; i < 4; i++)
  {
    double cDec = cos(-rd[i].Dec);
    SKVECTOR r;

    r.x = cDec * sin(-rd[i].Ra);
    r.y = sin(-rd[i].Dec);
    r.z = cDec * cos(-rd[i].Ra);

    SKVECTransform(&r, &r, &mat);

    rd[i].Ra  = atan2(r.z, r.x) - R90;
    rd[i].Dec = -atan2(r.y, sqrt(r.x * r.x + r.z * r.z));
    rangeDbl(&rd[i].Ra, R360);

    drw->frmField_t.corner[i].Ra = rd[i].Ra;
    drw->frmField_t.corner[i].Dec = rd[i].Dec;
  }
}

void calcAngularDistance(double ra, double dec, double angle, double distance, double &raOut, double &decOut)
{
  // http://www.movable-type.co.uk/scripts/latlong.html

  decOut = asin(sin(dec) * cos(distance) + cos(dec) * sin(distance) * cos(-angle));
  raOut = ra + atan2(sin(-angle) * sin(distance) * cos(dec), cos(distance) - sin(dec) * sin(decOut));
}

//////////////////////////////////////////////////////////////////////////////////////
int CDrawing::drawFrmField(QPoint &ptOut, CSkPainter *p, drawing_t *drw, bool bEdited)
//////////////////////////////////////////////////////////////////////////////////////
{
  SKPOINT pt;
  double  r2;
  int     r = 0;
  SKPOINT pp[4];

  if (bEdited)
  {
    m_drawing.onScr = false;
  }

  double aspect = drw->frmField_t.x / drw->frmField_t.y;
  double angle = D2R(drw->angle);

  double ang1 = -atan(aspect) + angle;
  double ang2 = atan(aspect) + angle;
  double ang3 = MPI - atan(aspect) + angle;
  double ang4 = MPI + atan(aspect) + angle;
  radec_t textRD;

  double dist = sqrt(POW2(drw->frmField_t.x) + POW2(drw->frmField_t.y)) * 0.5;

  calcAngularDistance(drw->rd.Ra, drw->rd.Dec, ang1, dist, drw->frmField_t.corner[0].Ra, drw->frmField_t.corner[0].Dec);
  calcAngularDistance(drw->rd.Ra, drw->rd.Dec, ang2, dist, drw->frmField_t.corner[1].Ra, drw->frmField_t.corner[1].Dec);
  calcAngularDistance(drw->rd.Ra, drw->rd.Dec, ang3, dist, drw->frmField_t.corner[2].Ra, drw->frmField_t.corner[2].Dec);
  calcAngularDistance(drw->rd.Ra, drw->rd.Dec, ang4, dist, drw->frmField_t.corner[3].Ra, drw->frmField_t.corner[3].Dec);

  trfRaDecToPointNoCorrect(&drw->frmField_t.corner[0], &pp[0]);
  trfRaDecToPointNoCorrect(&drw->frmField_t.corner[1], &pp[1]);
  trfRaDecToPointNoCorrect(&drw->frmField_t.corner[2], &pp[2]);
  trfRaDecToPointNoCorrect(&drw->frmField_t.corner[3], &pp[3]);

  p->setPen(QPen(QColor(g_skSet.map.drawing.color), 2));
  p->setBrush(Qt::NoBrush);

  //trfProjectPointNoCheck(&pp[0]);
  //trfProjectPointNoCheck(&pp[1]);
  //trfProjectPointNoCheck(&pp[2]);
  //trfProjectPointNoCheck(&pp[3]);
  double textAngle = CM_UNDEF;
  bool   showText = false;

  r2 = qMax(drw->frmField_t.x, drw->frmField_t.y);

  trfRaDecToPointNoCorrect(&drw->rd, &pt);
  if (SKPLANECheckFrustumToSphere(m_frustum, &pt.w, r2))
  {
    //SKPOINT origPts[4] = {pp[0], pp[1], pp[2], pp[3]};
    SKPOINT newPts[4];
    QList <QPoint> bndBox;

    if (trfProjectLine(&pp[0], &pp[1], newPts[0], newPts[1]))
    {
      p->drawLine(newPts[0].sx, newPts[0].sy, newPts[1].sx, newPts[1].sy);
      bndBox.append(QPoint(newPts[0].sx, newPts[0].sy));
      bndBox.append(QPoint(newPts[1].sx, newPts[1].sy));
    }

    if (trfProjectLine(&pp[1], &pp[2], newPts[1], newPts[2]))
    {
      p->drawLine(newPts[1].sx, newPts[1].sy, newPts[2].sx, newPts[2].sy);
      bndBox.append(QPoint(newPts[2].sx, newPts[2].sy));
      bndBox.append(QPoint(newPts[1].sx, newPts[1].sy));
    }

    if (trfProjectLine(&pp[2], &pp[3], newPts[2], newPts[3]))
    {
      p->drawLine(newPts[2].sx, newPts[2].sy, newPts[3].sx, newPts[3].sy);
      bndBox.append(QPoint(newPts[2].sx, newPts[2].sy));
      bndBox.append(QPoint(newPts[3].sx, newPts[3].sy));
      textAngle = -R2D(atan2(newPts[2].sx - newPts[3].sx, newPts[2].sy - newPts[3].sy)) - 270;
      showText = true;
    }

    if (trfProjectLine(&pp[3], &pp[0], newPts[3], newPts[0]))
    {
      p->drawLine(newPts[3].sx, newPts[3].sy, newPts[0].sx, newPts[0].sy);
      bndBox.append(QPoint(newPts[3].sx, newPts[3].sy));
      bndBox.append(QPoint(newPts[0].sx, newPts[0].sy));
    }      

    QRect rc;

    int minx = 99999, maxx = -99999;
    int miny = 99999, maxy = -99999;

    foreach (QPoint p, bndBox)
    {
      if (p.x() < minx) minx = p.x();
      if (p.x() > maxx) maxx = p.x();

      if (p.y() < miny) miny = p.y();
      if (p.y() > maxy) maxy = p.y();
    }

    rc.setX(minx);
    rc.setY(miny);
    rc.setRight(maxx);
    rc.setBottom(maxy);

    if (bEdited)
    {
      m_drawing.onScr = true;
      m_drawing.rect = rc;

      p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
      p->drawRect(m_drawing.rect);
      rc = m_drawing.rect.adjusted(ROT_MARGIN, ROT_MARGIN, -ROT_MARGIN, -ROT_MARGIN);
      p->drawRect(rc);
    }

    if (qMax(rc.width(), rc.height()) > 50)
    {
      setSetFontColor(FONT_DRAWING, p);
      setSetFont(FONT_DRAWING, p);

      QRect  textRect;
      QFontMetrics fm(p->font());
      SKPOINT textPoint;

      //double textAngle = R2D(atan2(pp[0].sx - pp[1].sx, pp[0].sy - pp[1].sy)) - 270;

      textRD = drw->rd;

      trfRaDecToPointNoCorrect(&textRD, &textPoint);
      if (trfProjectPoint(&textPoint))
      {
        p->save();
        p->translate(textPoint.sx, textPoint.sy);
        p->rotate(textAngle);
        textRect.setSize(QSize(1000, 1000));
        textRect.moveCenter(QPoint(0, 0));
        p->drawText(textRect, Qt::AlignCenter,
                    QString("%1' x %2'").arg(R2D(drw->frmField_t.x * 60), 0, 'f', 2).arg(R2D(drw->frmField_t.y * 60), 0, 'f', 2));
        p->restore();
      }
    }
  }

  return(r);
}

