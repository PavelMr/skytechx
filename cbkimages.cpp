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

//////////////////////
CBkImages::CBkImages()
//////////////////////
{
  m_totalSize = 0;
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
bool CBkImages::load(const QString name, int resizeTo)
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

    //qDebug("load '%s'", qPrintable(name));
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

  // TODO: pridat dialog pro rescale obrazku

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
          //qDebug("%s", qPrintable(i.filePath));
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



