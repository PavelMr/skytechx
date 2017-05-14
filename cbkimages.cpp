#include "cbkimages.h"
#include "cscanrender.h"
#include "transform.h"
#include "mainwindow.h"
#include "cmapview.h"
#include "setting.h"

extern MainWindow *pcMainWnd;
extern CMapView   *pcMapView;
extern bool g_showDSSFrameName;

CBkImages    bkImg;

const double editControlSize = 10;
const double axisLen = 60;
const double centerRadius = 4;

//////////////////////
CBkImages::CBkImages()
//////////////////////
{
  m_totalSize = 0;
  m_editFit = nullptr;
}

///////////////////////
CBkImages::~CBkImages()
///////////////////////
{
  for (int i = 0; i < m_tImgList.count(); i++)
  {
    deleteItem(0);
  }
}

//////////////////////////////////////////////////////
bool CBkImages::load(const QString name, int resizeTo, const radec_t &rdCenter, double fov)
//////////////////////////////////////////////////////
{
  QFileInfo fi(name);
  bkImgItem_t i;
  bool memOk;

  for (int i = 0; i < m_tImgList.count(); i++)
  {
    if (!m_tImgList[i].filePath.compare(fi.filePath(), Qt::CaseInsensitive))
    {
      return true; // already loaded
    }
  }

  if (!fi.suffix().compare("fits", Qt::CaseInsensitive))
  {
    CFits *f = new CFits;

    if (!f->load(name, memOk, true, resizeTo))
    {
      delete f;
      return(false);
    }

    i.bShow = true;
    i.filePath = name;
    i.byteSize = (int)fi.size(); // TODO: tohle je velikost originalu. V pameti to bude jinak (neco vymyslet)
    i.ptr = (void *)f;
    i.fileName = fi.fileName();
    i.type = BKT_DSSFITS;
    i.rd.Ra = f->m_ra;
    i.rd.Dec = f->m_dec;
    i.size = anSep(f->m_cor[0].Ra, f->m_cor[0].Dec, f->m_cor[2].Ra, f->m_cor[2].Dec);
    i.param.brightness = 0;
    i.param.contrast = 100;
    i.param.gamma = 150;
    i.param.invert = false;
    i.param.autoAdjust = false;
    i.param.useMatrix = false;
    memset(i.param.matrix, 0, sizeof(i.param.matrix));
    i.param.matrix[1][1] = 1;
    i.param.dlgSize = resizeTo;

    m_totalSize += i.byteSize;

    m_tImgList.append(i);
    pcMainWnd->updateDSS();    
  }
  else
  {
    CFits *f = new CFits;

    f->m_pix = new QImage(name);
    f->m_ori = new QImage(name);

    double aspect = f->m_pix->height() / (double)f->m_pix->width();
    f->m_ra = rdCenter.Ra;
    f->m_dec = rdCenter.Dec;
    f->m_width = fov * 0.25;
    f->m_height = f->m_width * aspect;
    f->m_angle = 0;

    f->m_controlPoint = rdCenter;
    f->cen_rd = rdCenter;
    f->m_name = name;

    i.bShow = true;
    i.filePath = name;
    i.byteSize = (int)fi.size(); // TODO: tohle je velikost originalu. V pameti to bude jinak (neco vymyslet)
    i.ptr = (void *)f;
    i.fileName = fi.fileName();
    i.type = BKT_CUSTOM;
    i.rd.Ra = f->m_ra;
    i.rd.Dec = f->m_dec;
    i.size = anSep(f->m_cor[0].Ra, f->m_cor[0].Dec, f->m_cor[2].Ra, f->m_cor[2].Dec);
    i.param.brightness = 0;
    i.param.contrast = 100;
    i.param.gamma = 150;
    i.param.invert = false;
    i.param.autoAdjust = false;
    i.param.useMatrix = false;
    memset(i.param.matrix, 0, sizeof(i.param.matrix));
    i.param.matrix[1][1] = 1;
    i.param.dlgSize = resizeTo;

    m_totalSize += i.byteSize;

    m_tImgList.append(i);
    pcMainWnd->updateDSS();
  }

  return(true);
}

/////////////////////////////////////////////////////////////////////
void CBkImages::loadOnScreen(QWidget *parent, double, double, double)
/////////////////////////////////////////////////////////////////////
{
  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/", "*.fits");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();  

  QProgressDialog dlg(tr("Please wait..."), tr("Cancel"), 0, list.count() - 1, parent);
  dlg.setWindowModality(Qt::WindowModal);
  dlg.setMinimumDuration(0);
  dlg.show();

  for (int i = 0; i < list.count(); i++)
  {
    bool memOk;
    bool found = false;
    dlg.setValue(i);
    QApplication::processEvents();

    QFileInfo fi = list.at(i);

    if (dlg.wasCanceled())
    {
      pcMainWnd->updateDSS();
      return;
    }

    for (int i = 0; i < m_tImgList.count(); i++)
    {
      if (!m_tImgList[i].filePath.compare(fi.filePath(), Qt::CaseInsensitive))
      {
        found = true;
        break;
      }
    }

    if (found)
      continue;

    CFits *f = new CFits;
    if (!f->load(fi.filePath(), memOk, false))
    {
      delete f;
    }
    else
    {
      if (pcMapView->isRaDecOnScreen(f->m_ra, f->m_dec))
      {
        bkImgItem_t i;
        delete f;
        bool memOk;

        CFits *f = new CFits;
        if (f->load(fi.filePath(), memOk))
        {
          i.bShow = true;
          i.filePath = fi.filePath();         
          i.byteSize = (int)fi.size();
          i.ptr = (void *)f;
          i.fileName = fi.fileName();
          i.type = BKT_DSSFITS;
          i.rd.Ra = f->m_ra;
          i.rd.Dec = f->m_dec;
          i.size = anSep(f->m_cor[0].Ra, f->m_cor[0].Dec, f->m_cor[2].Ra, f->m_cor[2].Dec);
          i.param.brightness = 0;
          i.param.contrast = 100;
          i.param.gamma = 150;
          i.param.invert = false;
          i.param.autoAdjust = false;
          i.param.dlgSize = 0;
          i.param.useMatrix = false;
          memset(i.param.matrix, 0, sizeof(i.param.matrix));
          i.param.matrix[1][1] = 1;

          m_totalSize += i.byteSize;

          m_tImgList.append(i);
          //pcMainWnd->updateDSS();
        }
      }
      else
      {
        delete f;
      }
    }
  }

  pcMainWnd->updateDSS();
}


////////////////////////////////////////////////////////////////////
void CBkImages::renderDSSFits(QImage *pDst, CSkPainter *p, CFits *fit)
////////////////////////////////////////////////////////////////////
{
  SKPOINT pt[4];

  for (int i = 0; i < 4; i++)
  {
    trfRaDecToPointNoCorrect(&fit->m_cor[i], &pt[i]);
  }

  if (!SKPLANECheckFrustumToPolygon(trfGetFrustum(), pt, 4))
    return;

  for (int i = 0; i < 4; i++)
  {
    trfProjectPointNoCheck(&pt[i]);
  }

  if (abs(pt[0].sx - pt[1].sx) < 10 && abs(pt[0].sy - pt[1].sy) < 10)
  {
    return;
  }

  scanRender.resetScanPoly(pDst->width(), pDst->height());
  scanRender.scanLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy, 0, 1, 1, 1);
  scanRender.scanLine(pt[1].sx, pt[1].sy, pt[2].sx, pt[2].sy, 1, 1, 1, 0);
  scanRender.scanLine(pt[2].sx, pt[2].sy, pt[3].sx, pt[3].sy, 1, 0, 0, 0);
  scanRender.scanLine(pt[3].sx, pt[3].sy, pt[0].sx, pt[0].sy, 0, 0, 0, 1);
  scanRender.renderPolygon(pDst, fit->getImage());

  if (g_showDSSFrameName)
  {
    p->setPen(g_skSet.map.drawing.color);
    p->drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);
    p->drawLine(pt[1].sx, pt[1].sy, pt[2].sx, pt[2].sy);
    p->drawLine(pt[2].sx, pt[2].sy, pt[3].sx, pt[3].sy);
    p->drawLine(pt[3].sx, pt[3].sy, pt[0].sx, pt[0].sy);

    SKPOINT newPts[2];

    if (trfProjectLine(&pt[0], &pt[1], newPts[0], newPts[1]))
    {
      double textAngle = -R2D(atan2(newPts[1].sx - newPts[0].sx, newPts[1].sy - newPts[0].sy)) - 270;

      SKPOINT textPoint;
      radec_t textRD;

      textRD.Ra = fit->m_cor[0].Ra;
      textRD.Dec = fit->m_cor[0].Dec;

      trfRaDecToPointNoCorrect(&textRD, &textPoint);
      if (trfProjectPoint(&textPoint))
      {
        setSetFontColor(FONT_DRAWING, p);
        setSetFont(FONT_DRAWING, p);

        p->save();
        p->translate(textPoint.sx, textPoint.sy);
        p->rotate(textAngle);
        p->renderText(0, 0, 5, fit->m_name, RT_TOP_RIGHT);
        p->restore();
      }
    }
  }
}

void CBkImages::renderCustomFits(QImage *pDst, CSkPainter *p, CFits *fit)
{
  SKPOINT pt;
  double  r2;
  int     r = 0;
  SKPOINT pp[4];
  radec_t corner[4];
  radec_t center = radec_t(fit->m_ra, fit->m_dec);

  double aspect = fit->m_width / fit->m_height;
  double angle = D2R(fit->m_angle);

  double ang1 = -atan(aspect) + angle;
  double ang2 = atan(aspect) + angle;
  double ang3 = MPI - atan(aspect) + angle;
  double ang4 = MPI + atan(aspect) + angle;
  radec_t textRD;

  double dist = sqrt(POW2(fit->m_width) + POW2(fit->m_height)) * 0.5;

  calcAngularDistance(fit->m_ra, fit->m_dec, ang1, dist, corner[0].Ra, corner[0].Dec);
  calcAngularDistance(fit->m_ra, fit->m_dec, ang2, dist, corner[1].Ra, corner[1].Dec);
  calcAngularDistance(fit->m_ra, fit->m_dec, ang3, dist, corner[2].Ra, corner[2].Dec);
  calcAngularDistance(fit->m_ra, fit->m_dec, ang4, dist, corner[3].Ra, corner[3].Dec);

  //calcAngularDistance(fit->m_controlPoint.Ra, fit->m_controlPoint.Dec, ang1, dist, corner[0].Ra, corner[0].Dec);
  //calcAngularDistance(fit->m_controlPoint.Ra, fit->m_controlPoint.Dec, ang2, dist, corner[1].Ra, corner[1].Dec);
  //calcAngularDistance(fit->m_controlPoint.Ra, fit->m_controlPoint.Dec, ang3, dist, corner[2].Ra, corner[2].Dec);
  //calcAngularDistance(fit->m_controlPoint.Ra, fit->m_controlPoint.Dec, ang4, dist, corner[3].Ra, corner[3].Dec);

  trfRaDecToPointNoCorrect(&corner[0], &pp[0]);
  trfRaDecToPointNoCorrect(&corner[1], &pp[1]);
  trfRaDecToPointNoCorrect(&corner[2], &pp[2]);
  trfRaDecToPointNoCorrect(&corner[3], &pp[3]);

  p->setPen(QPen(QBrush(g_skSet.map.drawing.color), g_skSet.map.drawing.width, (Qt::PenStyle)g_skSet.map.drawing.style));
  p->setBrush(Qt::NoBrush);

  double textAngle = CM_UNDEF;
  bool   showText = false;

  r2 = qMax(fit->m_width, fit->m_height);

  trfRaDecToPointNoCorrect(&center, &pt);
  if (SKPLANECheckFrustumToSphere(trfGetFrustum(), &pt.w, r2))
  {
    SKPOINT newPts[4];
    QList <QPoint> bndBox;

    if (g_showDSSFrameName)
    {
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
    }

    for (int i = 0; i < 4; i++)
    {
      trfProjectPointNoCheck(&pp[i]);
    }

    scanRender.resetScanPoly(pDst->width(), pDst->height());
    scanRender.scanLine(pp[0].sx, pp[0].sy, pp[1].sx, pp[1].sy, 0, 0, 1, 0);
    scanRender.scanLine(pp[1].sx, pp[1].sy, pp[2].sx, pp[2].sy, 1, 0, 1, 1);
    scanRender.scanLine(pp[2].sx, pp[2].sy, pp[3].sx, pp[3].sy, 1, 1, 0, 1);
    scanRender.scanLine(pp[3].sx, pp[3].sy, pp[0].sx, pp[0].sy, 0, 1, 0, 0);
    scanRender.renderPolygon(pDst, fit->getImage());

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

    trfRaDecToPointNoCorrect(&center, &pt);
    if (trfProjectPoint(&pt) && qMax(rc.width(), rc.height()) > 50)
    {
      p->save();
      p->setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
      p->drawCross(pt.sx, pt.sy, 8);
      p->restore();
    }

    if (fit->getEdited())
    {
      SKPOINT cxy;

      trfRaDecToPointNoCorrect(&fit->m_controlPoint, &cxy);
      trfProjectPointNoCheck(&cxy);

      double cx = cxy.sx;
      double cy = cxy.sy;

      p->drawLine(cx, cy - centerRadius, cx, cy - axisLen + editControlSize);
      p->drawCircle(QPoint(cx, cy - axisLen), editControlSize);

      p->drawLine(cx + centerRadius, cy, cx + axisLen - editControlSize, cy);
      p->drawCircle(QPoint(cx + axisLen, cy), editControlSize);
      p->drawCircle(QPoint(cx, cy), centerRadius);
    }

    if (qMax(rc.width(), rc.height()) > 50 && g_showDSSFrameName)
    {
      setSetFontColor(FONT_DRAWING, p);
      setSetFont(FONT_DRAWING, p);

      SKPOINT textPoint;

      textRD.Ra = corner[3].Ra;
      textRD.Dec = corner[3].Dec;

      trfRaDecToPointNoCorrect(&textRD, &textPoint);
      if (trfProjectPoint(&textPoint))
      {
        p->save();
        p->translate(textPoint.sx, textPoint.sy);
        p->rotate(textAngle);
        p->renderText(0, 0, 5, fit->m_name, RT_TOP_RIGHT);
        p->restore();
      }
    }
  }
}

int CBkImages::editObject(QPoint pos, QPoint delta, int op)
{
  if (m_editFit == nullptr || !m_editFit->getEdited())
  {
    return(DTO_NONE);
  }

  if (op == DTO_NONE)
  {
    SKPOINT cxy;

    trfRaDecToPointNoCorrect(&m_editFit->m_controlPoint, &cxy);
    trfProjectPointNoCheck(&cxy);

    QPoint center = QPoint(cxy.sx, cxy.sy);
    if (QPoint(pos - center).manhattanLength() < editControlSize)
    {
      if (QApplication::keyboardModifiers() & Qt::CTRL)
        return(DTO_MOVE_CTRL);
      else
        return(DTO_MOVE);
    }

    center = QPoint(cxy.sx, cxy.sy - axisLen);
    if (QPoint(pos - center).manhattanLength() < editControlSize)
    {
      return(DTO_ROTATE);
    }

    center = QPoint(cxy.sx + axisLen, cxy.sy);
    if (QPoint(pos - center).manhattanLength() < editControlSize)
    {
      return(DTO_SCALE);
    }

    return DTO_NONE;
  }

  if (op == DTO_MOVE || op == DTO_MOVE_CTRL)
  {
    double ra, dec;
    double ra2, dec2;

    trfConvScrPtToXY(pos.x(), pos.y(), ra, dec);
    trfConvScrPtToXY(pos.x() + delta.x(), pos.y() + delta.y(), ra2, dec2);

    precess(&ra, &dec, pcMapView->m_mapView.jd, JD2000);
    precess(&ra2, &dec2, pcMapView->m_mapView.jd, JD2000);

    if (op == DTO_MOVE)
    {
      m_editFit->m_ra -= ra2 - ra;
      m_editFit->m_dec -= dec2 - dec;
    }

    m_editFit->m_controlPoint.Ra -= ra2 - ra;
    m_editFit->m_controlPoint.Dec -= dec2 - dec;

    return(DTO_MOVE);
  }
  else
  if (op == DTO_ROTATE)
  {
    double dist =  anSep(m_editFit->m_ra, m_editFit->m_controlPoint.Ra,
                         m_editFit->m_dec, m_editFit->m_controlPoint.Dec);

    double ang = trfGetPosAngle(m_editFit->m_ra, m_editFit->m_controlPoint.Ra,
                                m_editFit->m_dec, m_editFit->m_controlPoint.Dec) + R270;

    qDebug() << m_editFit->m_ra - m_editFit->m_controlPoint.Ra;
    qDebug() << m_editFit->m_dec - m_editFit->m_controlPoint.Dec;

    //calcAngularDistance(m_editFit->m_controlPoint.Ra, m_editFit->m_controlPoint.Dec, ang + delta.x() * 0.25, dist, m_editFit->m_ra, m_editFit->m_dec);
    calcAngularDistance(m_editFit->m_controlPoint.Ra, m_editFit->m_controlPoint.Dec, ang + delta.x() * 0.25, dist, m_editFit->m_ra, m_editFit->m_dec);

    m_editFit->m_angle -= delta.x() * 0.25;

    //qDebug() << dist << ang;
    return(DTO_ROTATE);
  }
  else
  if (op == DTO_SCALE)
  {
    double scale;

    if (delta.x() < 0)
    {
      scale = 1.005 + qAbs(delta.x() * 0.001);
      if (qMax(m_editFit->m_width, m_editFit->m_height) > R90 * 0.5)
      {
        return DTO_SCALE;
      }
    }
    else
      scale = 0.995 - qAbs(delta.x() * 0.001);

    m_editFit->m_width *= scale;
    m_editFit->m_height *= scale;
    return(DTO_SCALE);
  }

  return(DTO_NONE);
}


/////////////////////////////////////////////////////////////
void CBkImages::renderAll(QImage *pDst, CSkPainter *pPainter)
/////////////////////////////////////////////////////////////
{
  foreach (const bkImgItem_t& i,  m_tImgList)
  {
    if (!i.bShow)
      continue;

    if (i.type == BKT_DSSFITS)
      renderDSSFits(pDst, pPainter, (CFits *)i.ptr);
    else if (i.type == BKT_CUSTOM)
      renderCustomFits(pDst, pPainter, (CFits *)i.ptr);
  }
}

/////////////////////////////////////
void CBkImages::deleteItem(int index)
/////////////////////////////////////
{
  if (m_tImgList[index].type == BKT_DSSFITS)
  {
    m_totalSize -= m_tImgList[index].byteSize;
    CFits *f = (CFits *)m_tImgList[index].ptr;
    delete f;
  }

  bkImg.m_tImgList.removeAt(index);
}

void CBkImages::setEdit(CFits *fit)
{
  m_editFit = fit;
}



