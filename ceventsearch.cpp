#include "ceventsearch.h"
#include "ui_ceventsearch.h"

#include "skcore.h"
#include "castro.h"
#include "jd.h"
#include "cgeohash.h"

#define GET_PERC(mi, ma, v)    (int)(((v - mi) / (double)(ma - mi) * 100.0))

QList <event_t *> tEventList;
int               nLastEventCount;

static int    evType = 0;
static bool   highPrec = true;
static bool   totLEcl = false;
static bool   totSEcl = false;
static double maxConjDist = 5;
static QMutex mutex;
static int    eventCount;
static QDate  sFrom;
static QDate  sTo;
static bool   bFirst = true;

using namespace searchEvent;


//////////////////////////////////////////////////////////////////////////////////////////
static int getVisibility(int type, mapView_t *view, double jd1, double jd2, int m_id = -1)
//////////////////////////////////////////////////////////////////////////////////////////
{
  if (type == EVT_OCCULTATION)
  {
    orbit_t o, s;

    view->jd = jd1;
    cAstro.setParam(view);
    cAstro.calcPlanet(m_id, &o);
    cAstro.calcPlanet(PT_SUN, &s);
    bool oVis1 = o.lAlt > 0;
    bool sVis1 = s.lAlt + DMS2RAD(0, 0, s.sy / 2) > 0;

    view->jd = jd2;
    cAstro.setParam(view);
    cAstro.calcPlanet(m_id, &o);
    cAstro.calcPlanet(PT_SUN, &s);
    bool oVis2 = o.lAlt > 0;
    bool sVis2 = s.lAlt + DMS2RAD(0, 0, s.sy / 2) > 0;

    if (sVis1 && sVis2)
    { // sun above horizon
      return(EVV_NONE);
    }

    if (oVis1 && oVis2)
    {
      return(EVV_FULL);
    }

    if (oVis1 || oVis2)
    {
      return(EVV_PARTIAL);
    }

    return(EVV_NONE);
  }
  else
  if (type == EVT_ELONGATION || type == EVT_OPPOSITION || type == EVT_CONJUCTION)
  {
    orbit_t o, s;

    view->jd = jd1;
    cAstro.setParam(view);
    cAstro.calcPlanet(PT_SUN, &s);
    cAstro.calcPlanet(m_id, &o);

    bool sVis = s.lAlt + DMS2RAD(0, 0, s.sy / 2) > 0;
    bool oVis = o.lAlt > 0;

    if (!oVis)
      return(EVV_NONE);
    if (sVis)
      return(EVV_NONE);

    return(EVV_FULL);
  }
  else
  if (type == EVT_SOLARECL || type == EVT_SUNTRANSIT)
  {
    orbit_t s1, s2;

    view->jd = jd1;
    cAstro.setParam(view);
    cAstro.calcPlanet(PT_SUN, &s1);

    view->jd = jd2;
    cAstro.setParam(view);
    cAstro.calcPlanet(PT_SUN, &s2);

    bool s1Vis = s1.lAlt + DMS2RAD(0, 0, s1.sy / 2) > 0;
    bool s2Vis = s2.lAlt + DMS2RAD(0, 0, s2.sy / 2) > 0;

    if (!s1Vis && !s2Vis)
      return(EVV_NONE);

    if ((s1Vis && !s2Vis) || (!s1Vis && s2Vis))
      return(EVV_PARTIAL);

    return(EVV_FULL);
  }
  else
  if (type == EVT_LUNARECL)
  {
    orbit_t s1, s2;

    view->jd = jd1;
    cAstro.setParam(view);
    cAstro.calcPlanet(PT_MOON, &s1);

    view->jd = jd2;
    cAstro.setParam(view);
    cAstro.calcPlanet(PT_MOON, &s2);

    bool s1Vis = s1.lAlt + DMS2RAD(0, 0, s1.sy / 2) > 0;
    bool s2Vis = s2.lAlt + DMS2RAD(0, 0, s2.sy / 2) > 0;

    if (!s1Vis && !s2Vis)
      return(EVV_NONE);

    if ((s1Vis && !s2Vis) || (!s1Vis && s2Vis))
      return(EVV_PARTIAL);

    return(EVV_FULL);
  }

  return(EVV_FULL);
}

CEventSearch::CEventSearch(QWidget *parent, mapView_t *view) :
    QDialog(parent),
    ui(new Ui::CEventSearch)
{
    ui->setupUi(this);

    m_view = *view;

    switch (evType)
    {
      case 0:
        ui->radioButton->setChecked(true);
        break;
      case 1:
        ui->radioButton_2->setChecked(true);
        break;
      case 2:
        ui->radioButton_3->setChecked(true);
        break;
      case 3:
        ui->radioButton_4->setChecked(true);
        break;
      case 4:
        ui->radioButton_5->setChecked(true);
        break;
      case 5:
        ui->radioButton_6->setChecked(true);
        break;
      case 6:
        ui->radioButton_7->setChecked(true);
        break;
      case 7:
        ui->radioButton_8->setChecked(true);
        break;
    }

    ui->checkBox->setChecked(highPrec);
    ui->checkBox_2->setChecked(totLEcl);
    ui->checkBox_3->setChecked(totSEcl);

    ui->spinBox->setValue(maxConjDist);

    ui->dateEdit->setMinimumDate(QDate(100, 1, 1));
    ui->dateEdit_2->setMinimumDate(QDate(100, 1, 1));

    QStandardItemModel *model = new QStandardItemModel(0, 1, NULL);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

    for (int i = 0; i < 9; i++)
    {
      QStandardItem *item = new QStandardItem;
      item->setText(cAstro.getName(i));
      item->setCheckable(true);
      item->setCheckState(Qt::Unchecked);
      item->setEditable(false);
      item->setData(i);
      model->appendRow(item);
    }

    ui->listView->setModel(model);

    setObjects();

    if (bFirst)
    {
      int y = (int)jdGetYearFromJD(view->jd);
      bFirst = false;
      sFrom = QDate(y, 1, 1);
      sTo = QDate(y, 12, 31);
    }

    ui->dateEdit->setDate(sFrom);
    ui->dateEdit_2->setDate(sTo);
}

CEventSearch::~CEventSearch()
{
    delete ui;
}


void CEventSearch::setObjects(void)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;

  ui->listView->selectionModel()->reset();

  for (int i = 0; i < 9; i++)
  {
    item = model->item(i);
    item->setCheckState(Qt::Unchecked);
    item->setEnabled(true);
  }

  switch (evType)
  {
    case 0: // solar ecl.
    case 1: // lunar ecl.
      ui->listView->setEnabled(false);
      item = model->item(PT_SUN);
      item->setCheckState(Qt::Checked);
      item = model->item(PT_MOON);
      item->setCheckState(Qt::Checked);
      break;

    case 7:
      ui->listView->setEnabled(false);
      item = model->item(PT_MOON);
      item->setCheckState(Qt::Checked);
      break;

    case 2: // conjuction
      ui->listView->setEnabled(true);
      item = model->item(PT_SUN);
      item->setEnabled(false);
      break;

   case 3: // opposition
      ui->listView->setEnabled(true);
      item = model->item(PT_MERCURY);
      item->setEnabled(false);
      item = model->item(PT_VENUS);
      item->setEnabled(false);
      item = model->item(PT_SUN);
      item->setEnabled(false);
      item = model->item(PT_MOON);
      item->setEnabled(false);
      break;

   case 4: // sun transit
      ui->listView->setEnabled(true);
      item = model->item(PT_MARS);
      item->setEnabled(false);
      item = model->item(PT_JUPITER);
      item->setEnabled(false);
      item = model->item(PT_SATURN);
      item->setEnabled(false);
      item = model->item(PT_URANUS);
      item->setEnabled(false);
      item = model->item(PT_NEPTUNE);
      item->setEnabled(false);
      item = model->item(PT_SUN);
      item->setEnabled(false);
      item = model->item(PT_MOON);
      item->setEnabled(false);
      break;

    case 5: // lunar occultation
      ui->listView->setEnabled(true);
      item = model->item(PT_SUN);
      item->setEnabled(false);
      item = model->item(PT_MOON);
      item->setEnabled(false);
      break;

    case 6: // max. elongation
      ui->listView->setEnabled(true);
      item = model->item(PT_MARS);
      item->setEnabled(false);
      item = model->item(PT_JUPITER);
      item->setEnabled(false);
      item = model->item(PT_SATURN);
      item->setEnabled(false);
      item = model->item(PT_URANUS);
      item->setEnabled(false);
      item = model->item(PT_NEPTUNE);
      item->setEnabled(false);
      item = model->item(PT_SUN);
      item->setEnabled(false);
      item = model->item(PT_MOON);
      item->setEnabled(false);
      break;
  }
}

void CEventSearch::changeEvent(QEvent *e)
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

void CEventSearch::on_radioButton_8_clicked()
{
  evType = 7;
  setObjects();
}

void CEventSearch::on_radioButton_7_clicked()
{
  evType = 6;
  setObjects();
}


void CEventSearch::on_radioButton_6_clicked()
{
  evType = 5;
  setObjects();
}

void CEventSearch::on_radioButton_5_clicked()
{
  evType = 4;
  setObjects();
}

void CEventSearch::on_radioButton_4_clicked()
{
  evType = 3;
  setObjects();
}

void CEventSearch::on_radioButton_3_clicked()
{
  evType = 2;
  setObjects();
}

void CEventSearch::on_radioButton_2_clicked()
{
  evType = 1;
  setObjects();
}

void CEventSearch::on_radioButton_clicked()
{
  evType = 0;
  setObjects();
}

void CEventSearch::on_pushButton_2_clicked()
{
  QDateTime t1(QDate(ui->dateEdit->date()), QTime(0, 0, 0));
  QDateTime t2(QDate(ui->dateEdit_2->date()), QTime(0, 0, 0));

  sFrom = ui->dateEdit->date();
  sTo = ui->dateEdit_2->date();

  jdFrom = jdGetJDFrom_DateTime(&t1);
  jdTo   = jdGetJDFrom_DateTime(&t2);

  if (jdFrom >= jdTo)
  {
    msgBoxError(this, tr("Invalid date interval!!!"));
    return;
  }

  highPrec = ui->checkBox->checkState();
  totLEcl = ui->checkBox_2->checkState();
  totSEcl = ui->checkBox_3->checkState();

  maxConjDist = ui->spinBox->value();

  bool bOk;

  switch (evType)
  {
    case 0:
      bOk = commonEvent(EVT_SOLARECL);
      break;

    case 1:
      bOk = commonEvent(EVT_LUNARECL);
      break;

    case 2:
      bOk = conjuction();
      break;

    case 3:
      bOk = opposition();
      break;

    case 4:
      bOk = commonEvent(EVT_SUNTRANSIT);
      break;

    case 5:
      bOk = commonEvent(EVT_OCCULTATION);
      break;

    case 6:
      bOk = maxElongation();
      break;

    case 7:
      bOk = biggestMoon();
      break;
  }

  if (!bOk)
  {
    for (int i = 0; i < tThread.count(); i++)
    {
      delete tThread[i];
    }
    tThread.clear();

    if (evType == 2)
      msgBoxError(this, tr("You must select two objects only!"));
    else
      msgBoxError(this, tr("No object is selected!"));
    return;
  }

  CEventProgDlg dlg(this);

  dlg.setMaxThreads(tThread.count());

  for (int i = 0; i < tThread.count(); i++)
  {
    connect(tThread[i], SIGNAL(sigDone()), &dlg, SLOT(slotThreadDone()));
    connect(tThread[i], SIGNAL(sigProgress(int,int)), &dlg, SLOT(slotProgress(int,int)));
    tThread[i]->start();
  }

  int firstIndex = tEventList.count();
  eventCount = 0;

  qDebug("start");
  QTime tt;

  tt.start();

  if (dlg.exec())
  { // ok

  }
  else
  { // cancel
    for (int i = 0; i < tThread.count(); i++)
    {
      tThread[i]->m_end = true;
    }

    int c;
    do
    { // wait to finish all threads
      c = tThread.count();
      for (int i = 0; i < tThread.count(); i++)
      {
        if (tThread[i]->isFinished())
          c--;
      }
    } while (c != 0);

    for (int i = 0; i < eventCount; i++)
      tEventList.removeLast();

    for (int i = 0; i < tThread.count(); i++)
      delete tThread[i];
    tThread.clear();

    return;
  }

  int c;
  do
  { // wait to finish all threads
    c = tThread.count();
    for (int i = 0; i < tThread.count(); i++)
    {
      if (tThread[i]->isFinished())
      {
        c--;
        qDebug("thread %d end", i);
      }
    }
  } while (c != 0);


  for (int i = 0; i < tThread.count(); i++)
    delete tThread[i];
  tThread.clear();


  qDebug("end = %d ms", tt.elapsed());

  /*
  // save list
  SkFile f("events/default.dat");

  if (f.open(SkFile::WriteOnly))
  {
    int cnt = tEventList.count();
    f.write((char *)&cnt, sizeof(int));
    for (int i = 0; i < tEventList.count(); i++)
    {
      f.write((char *)tEventList[i], sizeof(event_t));
    }
    f.close();
  }
  */

  // show info
  QString str;

  if (eventCount == 0)
    str = tr("No event found!");
  else
  {
    str = QString(tr("%1 event(s) found!")).arg(eventCount);
  }

  msgBoxInfo(this, str);
  nLastEventCount = eventCount;

  if (eventCount > 0)
    done(1);
}


bool CEventSearch::maxElongation(void)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;
  int            c = 0;

  for (int i = PT_MERCURY; i <= PT_VENUS; i++)
  {
    item = model->item(i);
    if (item->checkState() == Qt::Checked)
    {
      CMaxElongation *t = new CMaxElongation;

      t->m_view = m_view;
      t->m_jdFrom = jdFrom;
      t->m_jdTo = jdTo;
      t->m_id = i;
      t->m_highPrec = highPrec;
      t->m_end = false;

      tThread.append(t);
      c++;
    }
  }

  return(c == 0 ? false : true);
}


bool CEventSearch::opposition(void)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;
  int            c = 0;

  for (int i = PT_MARS; i <= PT_NEPTUNE; i++)
  {
    item = model->item(i);
    if (item->checkState() == Qt::Checked)
    {
      COpposition *t = new COpposition;

      t->m_view = m_view;
      t->m_jdFrom = jdFrom;
      t->m_jdTo = jdTo;
      t->m_id = i;
      t->m_highPrec = highPrec;
      t->m_end = false;

      tThread.append(t);
      c++;
    }
  }

  return(c == 0 ? false : true);
}


bool CEventSearch::conjuction(void)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;
  QList          <int> idList;

  for (int i = PT_MERCURY; i <= PT_MOON; i++)
  {
    item = model->item(i);
    if (item->checkState() == Qt::Checked)
    {
      idList.append(i);
    }
  }

  CConjuction *t = new CConjuction;

  t->m_view = m_view;
  t->m_jdFrom = jdFrom;
  t->m_jdTo = jdTo;
  t->m_idList = idList;
  t->m_highPrec = highPrec;
  t->m_maxDist = DEG2RAD(maxConjDist);
  t->m_end = false;

  tThread.append(t);

  return(true);
}

bool CEventSearch::biggestMoon()
{
  CBigMoon *t = new CBigMoon;

  t->m_view = m_view;
  t->m_jdFrom = jdFrom;
  t->m_jdTo = jdTo;
  t->m_highPrec = highPrec;
  t->m_end = false;

  tThread.append(t);

  return true;
}

bool CEventSearch::commonEvent(int type)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;
  int            c = 0;

  if (type == EVT_SOLARECL)
  {
    CCommonEvent *t = new CCommonEvent;

    t->m_view = m_view;
    t->m_jdFrom = jdFrom;
    t->m_jdTo = jdTo;
    t->m_id0 = PT_SUN;
    t->m_id1 = PT_MOON;
    t->m_type = type;
    t->m_highPrec = highPrec;
    t->m_end = false;

    tThread.append(t);

    return(true);
  }
  else
  if (type == EVT_SUNTRANSIT)
  {
    for (int i = PT_MERCURY; i <= PT_VENUS; i++)
    {
      item = model->item(i);
      if (item->checkState() == Qt::Checked)
      {
        CCommonEvent *t = new CCommonEvent;

        t->m_view = m_view;
        t->m_jdFrom = jdFrom;
        t->m_jdTo = jdTo;
        t->m_id0 = PT_SUN;
        t->m_id1 = i;
        t->m_type = type;
        t->m_highPrec = highPrec;
        t->m_end = false;

        tThread.append(t);
        c++;
      }
    }
    return(c == 0 ? false : true);
  }
  else
  if (type == EVT_OCCULTATION)
  {
    for (int i = PT_MERCURY; i <= PT_NEPTUNE; i++)
    {
      item = model->item(i);
      if (item->checkState() == Qt::Checked)
      {
        CCommonEvent *t = new CCommonEvent;

        t->m_view = m_view;
        t->m_jdFrom = jdFrom;
        t->m_jdTo = jdTo;
        t->m_id0 = PT_MOON;
        t->m_id1 = i;
        t->m_type = type;
        t->m_highPrec = highPrec;
        t->m_end = false;

        tThread.append(t);
        c++;
      }
    }

    return(c == 0 ? false : true);
  }
  else
  if (type == EVT_LUNARECL)
  {
    CCommonEvent *t = new CCommonEvent;

    t->m_view = m_view;
    t->m_jdFrom = jdFrom;
    t->m_jdTo = jdTo;
    t->m_id0 = PT_SUN;
    t->m_id1 = PT_MOON;
    t->m_type = type;
    t->m_highPrec = highPrec;
    t->m_end = false;

    tThread.append(t);

    return(true);
  }

  return(true);
}

////// COMMON EVENTS ////////////////////////

#define H2JD(h)     (h / 24.0)

void searchEvent::CCommonEvent::findFirstLastContact(double from,
                                                     double *jdF,
                                                     double *jdL,
                                                     double *jdI1,
                                                     double *jdI2,
                                                     bool inner)
{
  double step = 1 / 24.0 / 60.0; // 1 min

  if (m_highPrec)
    step = 1 / 24.0 / 60.0 / 60.0; // 1 sec

  double add = step;
  orbit_t o0;
  orbit_t o1;
  double s1;
  double s2;
  double s3;
  CAstro cEphem;
  bool   first = false;
  bool   i1 = false;
  bool   i2 = false;
  bool   first2 = true;

  m_view.jd = from;
  qDebug(" start %f", from);

  cEphem.setParam(&m_view);
  cEphem.calcPlanet(m_id1, &o1);
  if (m_type == EVT_LUNARECL)
    cEphem.calcEarthShadow(&o0, &o1);
  else
    cEphem.calcPlanet(m_id0, &o0);

  s2 = s3 = anSep(o0.lRD.Ra, o0.lRD.Dec,
                  o1.lRD.Ra, o1.lRD.Dec);

  while (true)
  {
    if (m_end)
      break; // forced quit

    cEphem.setParam(&m_view);
    cEphem.calcPlanet(m_id1, &o1);
    if (m_type == EVT_LUNARECL)
      cEphem.calcEarthShadow(&o0, &o1);
    else
      cEphem.calcPlanet(m_id0, &o0);

    s1 = anSep(o0.lRD.Ra, o0.lRD.Dec,
               o1.lRD.Ra, o1.lRD.Dec);

    double r;
    double ir;

    if (inner)
    {
      r = DEG2RAD(((o0.sx + o1.sx) / 2.0) / 3600.0);
      ir = DEG2RAD(((fabs(o0.sx - o1.sx)) / 2.0) / 3600.0);
    }
    else
    {
      r = DEG2RAD(((o0.sy + o1.sx) / 2.0) / 3600.0);
      ir = DEG2RAD(((fabs(o0.sy - o1.sx)) / 2.0) / 3600.0);
    }

    if (first2)
    {
      qDebug("   %f %f", r, ir);
      first2 = false;
    }

    if (!i1 && (s1 < ir && s2 > ir))
    { // first inner contact
      qDebug("  first i %f", m_view.jd - add);
      *jdI1 = m_view.jd;
      i1 = true;
    }

    if (!i2 && (s1 > ir && s2 < ir))
    { // last inner contact
      qDebug("  last i %f", m_view.jd - add);
      *jdI2 = m_view.jd;
      i2 = true;
    }

    if ((!first) && (s1 < r && s2 > r))
    { // first outer contact
      first = true;
      *jdF = m_view.jd;
      qDebug("  first %f", m_view.jd - add);
    }

    if (first && (s1 > r && s2 < r))
    { // last outer contact
      qDebug("  last %f", m_view.jd - add);
      *jdL = m_view.jd;
      break;
    }

    s3 = s2;
    s2 = s1;

    m_view.jd += add;
  }
}

// http://hemel.waarnemen.com/Computing/eclipse.html

void searchEvent::CCommonEvent::run(void)
{
  double  stepMin = 1 / 24.0 / 60.0; // 1 min
  double  step = 1 / 2.0; // 12hr
  double  add = step;
  orbit_t o0;
  orbit_t o1;
  int count = 0;
  double s1;
  double s2;
  double s3;
  CAstro cEphem;

  if (m_highPrec)
    stepMin = 1 / 24.0 / 60.0 / 60.0; // 1 sec

  qDebug("Common ids %d %d", m_id0, m_id1);

  m_view.jd = m_jdFrom;

  cEphem.setParam(&m_view);
  cEphem.calcPlanet(m_id1, &o1);
  if (m_type == EVT_LUNARECL)
    cEphem.calcEarthShadow(&o0, &o1);
  else
    cEphem.calcPlanet(m_id0, &o0);

  s2 = s3 = anSep(o0.lRD.Ra, o0.lRD.Dec,
                  o1.lRD.Ra, o1.lRD.Dec);

  while (true)
  {
    if (m_end)
      break; // forced quit

    cEphem.setParam(&m_view);
    cEphem.calcPlanet(m_id1, &o1);
    if (m_type == EVT_LUNARECL)
      cEphem.calcEarthShadow(&o0, &o1);
    else
      cEphem.calcPlanet(m_id0, &o0);

    s1 = anSep(o0.lRD.Ra, o0.lRD.Dec,
               o1.lRD.Ra, o1.lRD.Dec);

    if (s2 < s1 && s2 < s3)
    {
      double tmpJD = m_view.jd - add;

      m_view.jd -= add * 2;
      add /= 2.0;

      if (add <= stepMin)
      { // center of event
        double r = DEG2RAD(((o0.sx + o1.sx) / 2.0) / 3600.0);

        //qDebug("sss %f %f %d", s2, r, m_type);

        if (s2 < r)
        { //////////////////////////////////////////////////////////////////////////////////////////
          if (m_type == EVT_LUNARECL)
          {
            qDebug("lunar ecl. %f", tmpJD);

            m_event.event_u.lunarEcl_u.p2 = m_event.event_u.lunarEcl_u.p3 = -1;
            m_event.event_u.lunarEcl_u.u1 = m_event.event_u.lunarEcl_u.u2 = -1;
            m_event.event_u.lunarEcl_u.u3 = m_event.event_u.lunarEcl_u.u4 = -1;
            m_event.event_u.lunarEcl_u.type = EVLE_PARTIAL_PENUMBRA;

            findFirstLastContact(tmpJD - H2JD(5.0),
                                &m_event.event_u.lunarEcl_u.p1, &m_event.event_u.lunarEcl_u.p4,
                                &m_event.event_u.lunarEcl_u.p2, &m_event.event_u.lunarEcl_u.p3, true);

            if (m_event.event_u.lunarEcl_u.p2 != -1 && m_event.event_u.lunarEcl_u.p3 != -1)
              m_event.event_u.lunarEcl_u.type = EVLE_FULL_PENUMBRA;

            if (s2 < DEG2RAD(((o0.sy + o1.sx) / 2.0) / 3600.0))
            {
              m_event.event_u.lunarEcl_u.type = EVLE_PARTIAL_UMBRA;
              findFirstLastContact(tmpJD - H2JD(5.0),
                                  &m_event.event_u.lunarEcl_u.u1, &m_event.event_u.lunarEcl_u.u4,
                                  &m_event.event_u.lunarEcl_u.u2, &m_event.event_u.lunarEcl_u.u3, false);
              if (m_event.event_u.lunarEcl_u.u2 != -1 && m_event.event_u.lunarEcl_u.u3 != -1)
                m_event.event_u.lunarEcl_u.type = EVLE_FULL_UMBRA;
            }

            if (!totLEcl || (totLEcl && m_event.event_u.lunarEcl_u.type == EVLE_FULL_UMBRA))
            {
              event_t *e = new event_t;

              memcpy(e, &m_event, sizeof(event_t));

              e->type = EVT_LUNARECL;
              e->vis = getVisibility(e->type, &m_view, m_event.event_u.lunarEcl_u.p1, m_event.event_u.lunarEcl_u.p4);
              e->jd = tmpJD;
              e->geoHash = CGeoHash::calculate(&m_view.geo);
              e->locationName = m_view.geo.name;

              mutex.lock();
              tEventList.append(e);
              eventCount++;
              mutex.unlock();
            }
          }
          else
          ///////////////////////////////////////////////////////////////////////////////////////////////////////////
          if (m_type == EVT_OCCULTATION)
          {
            qDebug("occ %f", tmpJD);

            m_event.event_u.moonOcc_u.i1 = -1;
            m_event.event_u.moonOcc_u.i2 = -1;
            m_event.event_u.moonOcc_u.id = m_id1;

            // cas podle rychlosti mesice
            findFirstLastContact(tmpJD - H2JD(10.0),
                                &m_event.event_u.moonOcc_u.c1, &m_event.event_u.moonOcc_u.c2,
                                &m_event.event_u.moonOcc_u.i1, &m_event.event_u.moonOcc_u.i2);

            m_view.jd = tmpJD;
            cEphem.setParam(&m_view);
            cEphem.calcPlanet(m_id0, &o0);
            cEphem.calcPlanet(m_id1, &o1);

            event_t *e = new event_t;

            memcpy(e, &m_event, sizeof(event_t));

            e->type = EVT_OCCULTATION;
            e->vis = getVisibility(e->type, &m_view, m_event.event_u.moonOcc_u.c1, m_event.event_u.moonOcc_u.c2, m_id1);
            e->jd = tmpJD;
            e->id = m_id1;
            e->geoHash = CGeoHash::calculate(&m_view.geo);
            e->locationName = m_view.geo.name;

            mutex.lock();
            tEventList.append(e);
            eventCount++;
            mutex.unlock();
          }
          else ///////////////////////////////////////////////////////////////////////////////////////////////////////////
          if (m_type == EVT_SOLARECL)
          {
            qDebug("eclipse %f", tmpJD);

            m_event.event_u.solarEcl_u.i1 = -1;
            m_event.event_u.solarEcl_u.i2 = -1;

            findFirstLastContact(tmpJD - H2JD(3.5),
                                &m_event.event_u.solarEcl_u.c1, &m_event.event_u.solarEcl_u.c2,
                                &m_event.event_u.solarEcl_u.i1, &m_event.event_u.solarEcl_u.i2);

            if ((m_event.event_u.solarEcl_u.i1 == -1 && m_event.event_u.solarEcl_u.i1 != -1) ||
                (m_event.event_u.solarEcl_u.i1 != -1 && m_event.event_u.solarEcl_u.i1 == -1))
            {
              qDebug("CCommonEvent::run fail!!!!");
            }

            m_view.jd = tmpJD;
            cEphem.setParam(&m_view);
            cEphem.calcPlanet(m_id0, &o0);
            cEphem.calcPlanet(m_id1, &o1);

            if (m_event.event_u.solarEcl_u.i1 == -1 && m_event.event_u.solarEcl_u.i1 == -1)
            {
              // mag = (rs + rm - ?) / (2 rs)
              double rs = 1;
              double rm = o1.sx / o0.sx;
              double delta = 2 * RAD2DEG(s2) / (o0.sx / 3600.0);
              qDebug("delta = %f", delta);
              m_event.event_u.solarEcl_u.mag = (rs + rm - delta) / (2 * rs);
              m_event.event_u.solarEcl_u.type = EVE_PARTIAL;
            }
            else
            if (o0.sx <= o1.sx)
            {
              m_event.event_u.solarEcl_u.type = EVE_FULL;
              m_event.event_u.solarEcl_u.mag = o1.sx / o0.sy;
            }
            else
            {
              m_event.event_u.solarEcl_u.type = EVE_RING;
              m_event.event_u.solarEcl_u.mag = o1.sx / o0.sy;
            }

           if (!totSEcl || (totSEcl && m_event.event_u.solarEcl_u.mag >= 1.0))
            {
              event_t *e = new event_t;

              qDebug("sol %d %f", totSEcl, m_event.event_u.solarEcl_u.mag);

              memcpy(e, &m_event, sizeof(event_t));

              e->type = EVT_SOLARECL;
              e->vis = getVisibility(e->type, &m_view, m_event.event_u.solarEcl_u.c1, m_event.event_u.solarEcl_u.c2);
              e->jd = tmpJD;
              e->geoHash = CGeoHash::calculate(&m_view.geo);
              e->locationName = m_view.geo.name;

              mutex.lock();
              tEventList.append(e);
              eventCount++;
              mutex.unlock();
            }
          }
          else ///////////////////////////////////////////////////////////////////////////////////////////////////////////
          if (m_type == EVT_SUNTRANSIT)
          {
            if (o0.R > o1.R)
            { // je pred sluncem
              qDebug("transit %f", tmpJD);

              m_event.event_u.sunTransit_u.i1 = -1;
              m_event.event_u.sunTransit_u.i2 = -1;
              m_event.event_u.sunTransit_u.id = m_id1;

              findFirstLastContact(tmpJD - H2JD(12.0),
                                  &m_event.event_u.sunTransit_u.c1, &m_event.event_u.sunTransit_u.c2,
                                  &m_event.event_u.sunTransit_u.i1, &m_event.event_u.sunTransit_u.i2);

              m_view.jd = tmpJD;
              cEphem.setParam(&m_view);
              cEphem.calcPlanet(m_id0, &o0);
              cEphem.calcPlanet(m_id1, &o1);

              event_t *e = new event_t;

              memcpy(e, &m_event, sizeof(event_t));

              e->type = EVT_SUNTRANSIT;
              e->vis = getVisibility(e->type, &m_view, m_event.event_u.sunTransit_u.c1, m_event.event_u.sunTransit_u.c2);
              e->jd = tmpJD;
              e->geoHash = CGeoHash::calculate(&m_view.geo);
              e->locationName = m_view.geo.name;

              mutex.lock();
              tEventList.append(e);
              eventCount++;
              mutex.unlock();
            }
          }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        add = step;
        m_view.jd = tmpJD + add;
      }
    }

    s3 = s2;
    s2 = s1;

    m_view.jd += add;
    if (m_view.jd >= m_jdTo)
      break;

    if ((count % 10) == 0)
      sigProgress(GET_PERC(m_jdFrom, m_jdTo, m_view.jd), m_id1);
    count++;
  }

  emit sigDone();
}

///// MAX ELONGATION ////////////////////////

void searchEvent::CMaxElongation::run(void)
{
  orbit_t o;
  int     count = 0;
  double  aSep;
  double  pe1, pe2;
  double  add = 1 / 12.0; // 2hr
  CAstro  cEphem;
  event_t *e;

  m_view.jd = m_jdFrom;

  if (m_highPrec)
    add = 1 / 24.0; // 1hr

  cEphem.setParam(&m_view);
  cEphem.calcPlanet(m_id, &o);
  pe1 = pe2 = fabs(o.elongation);

  while (true)
  {
    if ((count % 10) == 0)
      sigProgress(GET_PERC(m_jdFrom, m_jdTo, m_view.jd), m_id);

    if (m_end)
      break; // forced quit

    cEphem.setParam(&m_view);
    cEphem.calcPlanet(m_id, &o);

    aSep = fabs(o.elongation);

    if (pe1 > pe2 && pe1 > aSep)
    {
      mutex.lock();

      e = new event_t;

      e->type = EVT_ELONGATION;
      e->vis = getVisibility(e->type, &m_view, m_view.jd, 0, m_id);
      e->id = m_id;
      e->jd = m_view.jd;
      e->event_u.elongation_u.elong = o.elongation;
      e->geoHash = CGeoHash::calculate(&m_view.geo);
      e->locationName = m_view.geo.name;

      tEventList.append(e);
      eventCount++;

      mutex.unlock();
    }

    pe2 = pe1;
    pe1 = aSep;

    m_view.jd += add;
    if (m_view.jd >= m_jdTo)
      break;
    count++;
  }

  //qDebug("count %d = %d", m_id, count);

  emit sigDone();
}



///// OPPOSITION ////////////////////////

void searchEvent::COpposition::run(void)
{
  orbit_t o;
  orbit_t s;
  int     count = 0;
  double  aSep;
  double  pe1;
  double  add = 1 / 12.0; // 2hr
  CAstro  cEphem;
  event_t *e;

  m_view.jd = m_jdFrom;

  if (m_highPrec)
    add = 1 / 24.0; // 1hr

  cEphem.setParam(&m_view);
  cEphem.calcPlanet(m_id, &o);
  cEphem.calcPlanet(PT_SUN, &s);

  double ol = o.hLon - R180;
  rangeDbl(&ol, R360);

  pe1 = sin(s.hLon - ol);
  // TODO: opravit ??????

  while (true)
  {
    if ((count % 10) == 0)
      sigProgress(GET_PERC(m_jdFrom, m_jdTo, m_view.jd), m_id);

    if (m_end)
      break; // forced quit

    cEphem.setParam(&m_view);
    cEphem.calcPlanet(m_id, &o);
    cEphem.calcPlanet(PT_SUN, &s);

    double ol = o.hLon - R180;
    rangeDbl(&ol, R360);

    aSep = sin(s.hLon - ol);

    if (aSep >= 0 && pe1 <= 0)
    {
      mutex.lock();

      e = new event_t;

      e->type = EVT_OPPOSITION;
      e->vis = getVisibility(e->type, &m_view, m_view.jd, 0, m_id);
      e->id = m_id;
      e->jd = m_view.jd;
      e->event_u.opposition_u.size = o.sx;
      e->event_u.opposition_u.R = o.R;
      e->geoHash = CGeoHash::calculate(&m_view.geo);
      e->locationName = m_view.geo.name;

      tEventList.append(e);
      eventCount++;

      mutex.unlock();
    }

    pe1 = aSep;

    m_view.jd += add;
    if (m_view.jd >= m_jdTo)
      break;
    count++;
  }

  emit sigDone();
}

//////// CONJUCTION //////////////////////////////////

void searchEvent::CConjuction::run(void)
{
  double  step = 1 / 12.0;
  double  add = step;
  orbit_t orbit[PT_PLANET_COUNT];
  int count = 0;
  double s1;
  double s2;
  double s3;
  double stepMin = 1 / 24.0 / 60.0;
  CAstro cEphem;

  if (m_highPrec)
    stepMin = 1 / 24.0 / 60.0 / 60;

  m_view.jd = m_jdFrom;

  cEphem.setParam(&m_view);

  foreach (int i, m_idList)
  {
    cEphem.calcPlanet(i, &orbit[i]);
  }

  double maxSep = 0;
  foreach (int i, m_idList)
  {
    foreach (int j, m_idList)
    {
      if (i == j)
      {
        continue;
      }
      double sep = anSep(orbit[i].lRD.Ra, orbit[i].lRD.Dec,
                         orbit[j].lRD.Ra, orbit[j].lRD.Dec);
      maxSep = qMax(sep, maxSep);
    }
  }
  s2 = s3 = maxSep;

  while (true)
  {
    if (m_end)
      break; // forced quit

    cEphem.setParam(&m_view);
    foreach (int i, m_idList)
    {
      cEphem.calcPlanet(i, &orbit[i]);
    }
    double maxSep = 0;
    foreach (int i, m_idList)
    {
      foreach (int j, m_idList)
      {
        if (i == j)
        {
          continue;
        }
        double sep = anSep(orbit[i].lRD.Ra, orbit[i].lRD.Dec,
                           orbit[j].lRD.Ra, orbit[j].lRD.Dec);
        maxSep = qMax(sep, maxSep);
      }
    }
    s1 = maxSep;

    //qDebug() << m_view.jd << s1 << s2 << s3;

    if (s2 < s1 && s2 < s3)
    {
      m_view.jd -= add * 2;
      add /= 2.0;

      if (add <= stepMin)
      {
        /*
        qDebug("found! %s", getStrDeg(s2).toAscii().data());
        qDebug(" d = %s", getStrDate(m_view.jd, 0).toAscii().data());
        qDebug(" t = %s", getStrTime(m_view.jd, 0).toAscii().data());
        */

        if (s2 <= m_maxDist)
        {
          int vis = EVV_FULL;

          /*
          if (o0.lAlt > 0 && o1.lAlt > 0)
            vis = EVV_FULL;
          else
          if (o0.lAlt > 0 || o1.lAlt > 0)
            vis = EVV_PARTIAL;
          else
            vis = EVV_NONE;
            */

          //if (!m_visOnly || (m_visOnly && vis >= EVV_PARTIAL))
          {
            event_t *e;

            mutex.lock();

            e = new event_t;
            e->type = EVT_CONJUCTION;
            e->vis = vis;
            e->id = -1;
            e->jd = m_view.jd;
            e->event_u.conjuction_u.dist = s2;

            int c = 0;
            foreach (int i, m_idList)
            {
              e->event_u.conjuction_u.idList[c] = i;
              c++;
            }

            e->event_u.conjuction_u.idCount = m_idList.count();
            e->geoHash = CGeoHash::calculate(&m_view.geo);
            e->locationName = m_view.geo.name;

            tEventList.append(e);
            eventCount++;

            mutex.unlock();
          }
        }

        add = step;
      }
    }

    s3 = s2;
    s2 = s1;

    m_view.jd += add;
    if (m_view.jd >= m_jdTo)
      break;

    if ((count % 10) == 0)
      sigProgress(GET_PERC(m_jdFrom, m_jdTo, m_view.jd), m_idList.at(0));
    count++;
  }
  emit sigDone();
}

//////////////////////////////////////////
void CEventSearch::on_pushButton_clicked()
//////////////////////////////////////////
{
  done(0);
}

////////////////////////////////////////////
void CEventSearch::on_pushButton_3_clicked()
////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;

  if (!ui->listView->isEnabled())
  {
    return;
  }

  for (int i = 0; i < model->rowCount(); i++)
  {
    item = model->item(i);
    if (item->isEnabled())
    {
      item->setCheckState(Qt::Checked);
    }
  }
}

////////////////////////////////////////////
void CEventSearch::on_pushButton_4_clicked()
////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QStandardItem *item;

  if (!ui->listView->isEnabled())
  {
    return;
  }

  for (int i = 0; i < model->rowCount(); i++)
  {
    item = model->item(i);
    item->setCheckState(Qt::Unchecked);
  }
}


void CBigMoon::run()
{
  double step = 1 / 4.0; // 6hr
  CAstro cEphem;
  int count = 0;
  orbit_t orbit;
  double phase1;
  double phase2;
  double phase3;
  double jd = m_jdFrom;

  if (m_highPrec)
  {
    step = 1 / 24.0; // 1hr
  }

  m_view.jd = m_jdFrom;
  cEphem.setParam(&m_view);
  cEphem.calcPlanet(PT_MOON, &orbit);
  phase1 = phase2 = qAbs(orbit.FV);

  while (true)
  {
    if ((count % 10) == 0)
    {
      sigProgress(GET_PERC(m_jdFrom, m_jdTo, m_view.jd), PT_MOON);
    }

    if (m_end)
    {
      break; // forced quit
    }

    cEphem.setParam(&m_view);
    cEphem.calcPlanet(PT_MOON, &orbit);
    phase3 = qAbs(orbit.FV);

    if (phase1 > phase2 && phase2 < phase3 && (orbit.R * 6371 < 358000.0)) // FIX: spravnou hodnotuS
    {
      mutex.lock();

      event_t *e;

      e = new event_t;
      e->type = EVT_BIG_MOON;
      e->vis = EVV_FULL;
      e->id = PT_MOON;
      e->jd = jd;
      e->geoHash = CGeoHash::calculate(&m_view.geo);
      e->locationName = m_view.geo.name;
      e->event_u.bigMoon_u.R = orbit.R;

      //qDebug() << getStrDate(jd, 0) << orbit.R;

      tEventList.append(e);
      eventCount++;
      mutex.unlock();
    }

    jd = m_view.jd;
    m_view.jd += step;
    if (m_view.jd >= m_jdTo)
    {
      break;
    }

    phase1 = phase2;
    phase2 = phase3;

    count++;
  }
  emit sigDone();
}

void CEventSearch::on_pushButton_5_clicked()
{
  ui->dateEdit->setDate(QDate::currentDate());
  ui->dateEdit_2->setDate(QDate::currentDate().addDays(365));
}
