#include "cobjtracking.h"
#include "ui_cobjtracking.h"
#include "casterdlg.h"
#include "ccomdlg.h"
#include "setting.h"
#include "csgp4.h"

QList <tracking_t> tTracking;
extern CMapView    *pcMapView;

///////////////////////
void loadTracking(void)
///////////////////////
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/tracking/tracking.dat");
  QDataStream s(&f);

  if (f.open(SkFile::ReadOnly))
  {
    int count;

    s >> count;

    for (int i = 0; i < count; i++)
    {
      tracking_t t;

      s >> t.show;
      s >> t.bShowDateTime;
      s >> t.bShowMag;
      s >> t.jdFrom;
      s >> t.jdTo;
      s >> t.labelAngle;
      s >> t.labelStep;
      s >> t.objName;
      s >> t.type;

      int c;

      s >> c;

      for (int j = 0; j < c; j++)
      {
        trackPos_t p;

        s >> p.jd;
        s >> p.mag;
        s >> p.rd.Ra;
        s >> p.rd.Dec;

        t.tPos.append(p);
      }
      tTracking.append(t);
    }
    f.close();
  }
}

///////////////////////
void saveTracking(void)
///////////////////////
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/tracking/tracking.dat");
  QDataStream s(&f);

  if (f.open(SkFile::WriteOnly))
  {
    s << tTracking.count();

    for (int i = 0; i < tTracking.count(); i++)
    {
      s << tTracking.at(i).show;
      s << tTracking.at(i).bShowDateTime;
      s << tTracking.at(i).bShowMag;
      s << tTracking.at(i).jdFrom;
      s << tTracking.at(i).jdTo;
      s << tTracking.at(i).labelAngle;
      s << tTracking.at(i).labelStep;
      s << tTracking.at(i).objName;
      s << tTracking.at(i).type;

      s << tTracking.at(i).tPos.count();

      for (int j = 0; j < tTracking.at(i).tPos.count(); j++)
      {
        s << tTracking.at(i).tPos[j].jd;
        s << tTracking.at(i).tPos[j].mag;
        s << tTracking.at(i).tPos[j].rd.Ra;
        s << tTracking.at(i).tPos[j].rd.Dec;
      }
    }

    f.close();
  }
}

/////////////////////////////
void deleteTracking(int type)
/////////////////////////////
{
  int i = 0;

  while(i < tTracking.count())
  {
    if (tTracking[i].type == type)
    {
      tTracking.removeAt(i);
    }
    else
    {
      i++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static float drawLineTicks(SKPOINT *p1, SKPOINT *p2, CSkPainter *pPainter, int size, SKPOINT *where)
////////////////////////////////////////////////////////////////////////////////////////////////////
{
  double x1, y1;
  double x2, y2;

  trfProjectPointNoCheckDbl(p1, x1, y1);
  trfProjectPointNoCheckDbl(p2, x2, y2);

  double dx = x2 - x1;
  double dy = y2 - y1;

  if (qAbs(dx) + qAbs(dy) != 0)
  {
    float ang = atan2(dx, dy);

    double i = 1 / sqrt(POW2(dx) + POW2(dy));
    dx *= i;
    dy *= i;

    pPainter->drawLine(where->sx - (dy * size), where->sy + (dx * size),
                       where->sx + (dy * size), where->sy - (dx * size));

    return(R2D(ang));
  }

  return(0);
}

///////////////////////////////////////////////////////
void trackRender(mapView_t *view, CSkPainter *pPainter)
///////////////////////////////////////////////////////
{
  SKPOINT p1;
  SKPOINT p2;

  int size = 4;
  int markPeriod = 5;

  for (int i = 0; i < tTracking.count(); i++)
  {
    if (!tTracking[i].show)
    {
      continue;
    }

    const trackPos_t *pos1;
    const trackPos_t *pos2;
    int   ls = tTracking[i].labelStep;
    bool  bDT = tTracking[i].bShowDateTime;
    bool  bMag = tTracking[i].bShowMag;
    float la = tTracking[i].labelAngle;

    for (int j = 0; j < tTracking[i].tPos.count() - 1; j++)
    {
      pos1 = &tTracking[i].tPos.at(j);
      trfRaDecToPointCorrectFromTo(&pos1->rd, &p1, view->jd, JD2000);

      pos2 = &tTracking[i].tPos.at(j + 1);
      trfRaDecToPointCorrectFromTo(&pos2->rd, &p2, view->jd, JD2000);

      if (trfProjectLine(&p1, &p2))
      {
        pPainter->setPen(QColor(g_skSet.map.tracking.color));
        pPainter->drawLine(p1.sx, p1.sy, p2.sx, p2.sy);

        if ((j % ls) == 0)
        {
          QString str;

          if (bDT)
            str += getStrDate(pos1->jd, view->geo.tz) + " " + getStrTime(pos1->jd, view->geo.tz, true) + " ";
          if (bMag && pos1->mag != CM_UNDEF)
            str += getStrMag(pos1->mag);

          pPainter->drawCross(p1.sx, p1.sy, 7);

          setSetFontColor(FONT_TRACKING, pPainter);
          setSetFont(FONT_TRACKING, pPainter);
          pPainter->drawRotatedText(la + R2D(view->roll), p1.sx, p1.sy, "  "  + str);
          pPainter->setPen(QColor(g_skSet.map.tracking.color));
        }
        else
        {
          if ((j % markPeriod) == 0)
            drawLineTicks(&p1, &p2, pPainter, size, &p1);
        }

        if (j + 2 == tTracking[i].tPos.count())
        { // last tick
          if (((j + 1) % ls) == 0)
          {
            QString str;

            if (bDT)
              str += getStrDate(pos2->jd, view->geo.tz) + " " + getStrTime(pos2->jd, view->geo.tz, true) + " ";
            if (bMag)
              str += getStrMag(pos2->mag);

            pPainter->drawCross(p2.sx, p2.sy, 7);

            setSetFontColor(FONT_TRACKING, pPainter);
            setSetFont(FONT_TRACKING, pPainter);
            pPainter->drawRotatedText(la + R2D(view->roll), p2.sx, p2.sy, "  " + str);
            pPainter->setPen(QColor(g_skSet.map.tracking.color));
          }
          else
          {
            if ((j % markPeriod) == 0)
              drawLineTicks(&p1, &p2, pPainter, size, &p2);
          }
        }
      }
      //pos1 = pos2;
      //p1 = p2;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
CObjTracking::CObjTracking(QWidget *parent, ofiItem_t *item, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CObjTracking),
  m_done(true)
///////////////////////////////////////////////////////////////////////////////
{
  ui->setupUi(this);
  setWindowTitle(tr("Track of ") + item->title);

  QDateTime dt;

  jdConvertJDTo_DateTime(view->jd + view->geo.tz, &dt);
  ui->dateTimeEdit->setDate(dt.date());
  ui->dateTimeEdit->setTime(dt.time());

  jdConvertJDTo_DateTime(view->jd + view->geo.tz + 10, &dt);
  ui->dateTimeEdit_2->setDate(dt.date());
  ui->dateTimeEdit_2->setTime(dt.time());

  ui->comboBox->addItem(tr("Second(s)"));
  ui->comboBox->addItem(tr("Minute(s)"));
  ui->comboBox->addItem(tr("Hour(s)"));
  ui->comboBox->addItem(tr("Day(s)"));
  ui->comboBox->setCurrentIndex(2);

  m_item = item;
  m_view = *view;  

  move(pcMapView->mapToGlobal(QPoint(pcMapView->x(), pcMapView->y())));
}

/////////////////////////////
CObjTracking::~CObjTracking()
/////////////////////////////
{
  delete ui;
}

/////////////////////////////////////////
void CObjTracking::changeEvent(QEvent *e)
/////////////////////////////////////////
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

////////////////////////////////////////////
// on OK
void CObjTracking::on_pushButton_2_clicked()
////////////////////////////////////////////
{
  QDateTime  dt = ui->dateTimeEdit->dateTime();
  double     jdFrom = jdGetJDFrom_DateTime(&dt) - m_view.geo.tz;
  QDateTime  dt2 = ui->dateTimeEdit_2->dateTime();
  double     jdTo = jdGetJDFrom_DateTime(&dt2) - m_view.geo.tz;
  int        step = ui->spinBox->value();
  int        tt = ui->comboBox->currentIndex();
  int        ls = ui->spinBox_2->value();
  double     jdStep;
  int        c = 0;
  orbit_t    o;
  tracking_t track;
  trackPos_t pos;
  asteroid_t *ast;
  comet_t    *com;
  satellite_t sat;

  if (jdFrom >= jdTo)
  {
    msgBoxError(this, tr("Invalid date range!"));
    return;
  }

  switch (tt)
  {
    case 0:
      jdStep = step;
      break;
    case 1:
      jdStep = step * 60.0;
      break;
    case 2:
      jdStep = step * 3600.0;
      break;
    case 3:
      jdStep = step * 86400.0;
      break;

    default:
      return;
  }

  jdStep /= 86400.0;

  // FIXME: zkontrolovat vsechny pouziti cAstro

  track.show = true;
  track.labelStep = ls;
  track.bShowDateTime = ui->checkBox->isChecked();
  track.bShowMag = ui->checkBox_2->isChecked();
  track.labelAngle = ui->spinBox_3->value();
  track.jdFrom = jdFrom;
  track.jdTo = jdTo;
  track.type = m_item->type;

  switch (m_item->type)
  {
    case MO_PLANET:
      track.objName = cAstro.getName(m_item->par1);
      break;

    case MO_COMET:
      com = (comet_t *)m_item->par2;
      track.objName = com->name;
      break;

    case MO_ASTER:
      ast = (asteroid_t *)m_item->par2;
      track.objName = ast->name;
      break;

    case MO_SATELLITE:
      track.objName = sgp4.getName(m_item->par1);
      break;
  }

  for (double j = jdFrom; j <= jdTo; j+= jdStep, c++)
  {
    m_view.jd = j;
    cAstro.setParam(&m_view);

    switch (m_item->type)
    {
      case MO_PLANET:
        cAstro.calcPlanet(m_item->par1, &o);
        pos.rd.Ra = o.lRD.Ra;
        pos.rd.Dec = o.lRD.Dec;
        pos.mag = o.mag;
        pos.jd = j;
        break;

      case MO_COMET:
        comSolve(com, j);
        pos.rd.Ra = com->orbit.lRD.Ra;
        pos.rd.Dec = com->orbit.lRD.Dec;
        pos.mag = com->orbit.mag;
        pos.jd = j;
        break;

      case MO_ASTER:
        astSolve(ast, j);
        pos.rd.Ra = ast->orbit.lRD.Ra;
        pos.rd.Dec = ast->orbit.lRD.Dec;
        pos.mag = ast->orbit.mag;
        pos.jd = j;
        break;

      case MO_SATELLITE:
        sgp4.solve(m_item->par1, &m_view, &sat);

        cAstro.convAA2RDRef(sat.azimuth, sat.elevation, &pos.rd.Ra, &pos.rd.Dec);
        pos.mag = CM_UNDEF;
        pos.jd = j;
        break;
    }
    track.tPos.append(pos);
  }

  if (track.tPos.count() < 2)
  {
    msgBoxError(this, tr("Tracking has too few points!"));
    return;
  }

  tTracking.append(track);

  if (m_done)
  {
    done(DL_OK);
  }
  m_done = true;
}


//////////////////////////////////////////
// on Cancel
void CObjTracking::on_pushButton_clicked()
//////////////////////////////////////////
{
  done(DL_CANCEL);
}

void CObjTracking::on_pushButton_3_clicked()
{
  m_done = false;
  on_pushButton_2_clicked();
  pcMapView->repaintMap();

  if (!tTracking.isEmpty())
  {
    tTracking.removeLast();
  }
}
