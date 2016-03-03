#include "cmooncal.h"
#include "ui_cmooncal.h"
#include "cplanetrenderer.h"
#include "crts.h"
#include "mapobj.h"

extern int g_ephType;
extern int g_ephMoonType;
extern bool g_geocentric;

///////////////////////////////////////////////////////
CMoonCal::CMoonCal(QWidget *parent, mapView_t *view) :
  QDialog(parent),
///////////////////////////////////////////////////////
  ui(new Ui::CMoonCal)
{
  m_img = NULL;

  ui->setupUi(this);

  m_moon = new QPixmap(":/res/moon_small.png");
  m_sun  = new QPixmap(":/res/sun_small.png");

  m_isMoon = true;

  QDateTime t;

  t = t.currentDateTime();

  m_curDay = t.date().day();
  m_curMonth = t.date().month();
  m_curYear = t.date().year();

  jdConvertJDTo_DateTime(view->jd + view->geo.tz, &t);
  m_year = t.date().year();
  m_month = t.date().month();

  m_img = new QImage(1, 1, QImage::Format_ARGB32);
  m_view = *view;

  ui->comboBox->addItem(tr("Moon"));
  ui->comboBox->addItem(tr("Sun2", "Sun (not Sunday)"));
  ui->comboBox->setCurrentIndex(0);

  updateTime();
}

/////////////////////
CMoonCal::~CMoonCal()
/////////////////////
{
  delete m_moon;
  delete m_sun;
  delete m_img;
  delete ui;
}

///////////////////////////////////////////
void CMoonCal::resizeEvent(QResizeEvent *)
///////////////////////////////////////////
{
  updateTime();
}

/////////////////////////////////////
void CMoonCal::changeEvent(QEvent *e)
/////////////////////////////////////
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

////////////////////////////////////////
void CMoonCal::paintEvent(QPaintEvent *)
////////////////////////////////////////
{
  QPainter p(this);
  QRect    rc;

  if (m_img->size() != ui->frame->size())
  {
    SAFE_DELETE(m_img);

    m_img = new QImage(ui->frame->size(), QImage::Format_ARGB32);
    updateTime();
  }

  rc.setTopLeft(ui->frame->pos() + ui->groupBox->pos());
  rc.setSize(ui->frame->size());
  p.drawImage(rc.left(), rc.top(), *m_img);
}


///////////////////////////
void CMoonCal::updateTime()
///////////////////////////
{
  QString months[12] = {tr("January (I)"),
                        tr("February (II)"),
                        tr("March (III)"),
                        tr("April (IV)"),
                        tr("May (V)"),
                        tr("June (VI)"),
                        tr("July (VII)"),
                        tr("August (IIX)"),
                        tr("September (IX)"),
                        tr("October (X)"),
                        tr("November (XI)"),
                        tr("December (XII)"),
                       };

  QDateTime t;
  QDateTime t1(QDate(m_year, m_month, 1));
  int cnt = 7 * 6;

  // first day
  int fd = (t1.date().dayOfWeek() - 1);
  cnt += fd;

  t.setDate(QDate(m_year, m_month, 1));
  t.setTime(QTime(12, 0, 0));
  t = t.addDays(-fd);

  ui->label->setText(QString("%1").arg(m_year));
  ui->label_2->setText(QString("%1").arg(months[m_month - 1]));

  int x = 0;
  int y = 0;
  float w = (m_img->width() - 1) / 7.f;
  float h = (m_img->height() - 1) / 6.f;

  double jd = jdGetJDFrom_DateTime(&t);

  m_img->fill(Qt::black);

  QPainter p(m_img);

  p.setRenderHint(QPainter::Antialiasing);

  float size = 0.5 * qMin(w, h) * 0.8f;

  int et = g_ephType;
  int mt = g_ephMoonType;
  bool gc = g_geocentric;

  g_ephType = EPT_PLAN404;
  g_ephMoonType = EPT_PLAN404;
  g_geocentric = false;


  for (int i = 0; i < cnt; i++)
  {
    QRect r;
    QRect rb;

    jdConvertJDTo_DateTime(jd, &t);
    int d = t.date().day();

    r.setRect(x * w + 2, y * h + 2, w - 4, h - 4);

    if (m_curDay == t.date().day() &&
        m_curMonth == t.date().month() &&
        m_curYear == t.date().year())
      p.setPen(QPen(QColor(255, 255, 0), 3));
    else
      p.setPen(QColor(255, 255, 255));
    p.setBrush(Qt::NoBrush);
    p.drawRect(r);

    CAstro  a;
    orbit_t o;
    orbit_t s;
    mapView_t view = m_view;
    SKPOINT   pt;

    view.flipX = false;
    view.flipY = false;
    view.jd = jd;
    a.setParam(&view);
    a.calcPlanet(PT_MOON, &o);
    a.calcPlanet(PT_SUN, &s);

    CRts   cRts;
    rts_t  rts;

    cRts.setLowPrec();

    if (m_isMoon)
      cRts.calcOrbitRTS(&rts, PT_MOON, MO_PLANET, &view);
    else
      cRts.calcOrbitRTS(&rts, PT_SUN, MO_PLANET, &view);

    pt.sx = r.center().x();
    pt.sy = r.center().y();

    rb = r;

    r.setRect(pt.sx - size * 0.98f, pt.sy - size * 0.98f, size * 2 * 0.98f, size * 2 * 0.98f);

    if (m_isMoon)
    {
      p.drawPixmap(r, *m_moon);
      p.save();
      cPlanetRenderer.drawPhase(&o, &s, &p, &pt, &view, size, size, false);
      p.restore();
    }
    else
    {
      p.drawPixmap(r, *m_sun);
    }

    if (t.date().month() != m_month)
      p.setPen(QColor(128, 128, 128));
    else
      p.setPen(QColor(255, 255, 255));

    p.setFont(QFont("arial", 12, QFont::Bold));
    p.drawText(rb.left() + 3, rb.top() + 3, 30, 30, Qt::AlignCenter, QString("%1").arg(d));

    p.setFont(QFont("arial", 10));
    p.setPen(QColor(255, 255, 255));

    float tz = view.geo.tz;
    QString str;

    switch (rts.flag)
    {
      case RTS_ERR:
        str = tr("Rise/Set solve ERROR!!!");
        break;

      case RTS_CIRC:
        str = tr("Object is circumpolar.\n");
        break;

      case RTS_NONV:
        str = tr("Object is never visible!\n");
        break;

      case RTS_DONE:
        if ((rts.rts & RTS_T_RISE) == RTS_T_RISE)
          str = tr("Rise : ") +  getStrTime(rts.rise, tz, true) + "\n";
        if ((rts.rts & RTS_T_SET) == RTS_T_SET)
          str += tr("Set : ") +  getStrTime(rts.set, tz, true);
        break;
    }

    p.setFont(QFont("arial", 8));
    p.setPen(QColor(255, 255, 255));
    p.drawText(rb.x() + 5, rb.y() + 5, rb.width() - 10, rb.height() - 10, Qt::AlignLeft | Qt::AlignBottom, QString("%1").arg(str));

    jd += 1;
    x++;
    if ((x % 7) == 0)
    {
      x = 0;
      y++;
    }
  }

  g_ephType = et;
  g_ephMoonType = mt;
  g_geocentric = gc;

  update();
}


//////////////////////////////////////
void CMoonCal::on_pushButton_clicked()
//////////////////////////////////////
{
  if (--m_year < 1)
    m_year = 1;
  updateTime();
}

////////////////////////////////////////
void CMoonCal::on_pushButton_4_clicked()
////////////////////////////////////////
{
  if (++m_year > 2999)
    m_year = 2999;
  updateTime();
}

////////////////////////////////////////
void CMoonCal::on_pushButton_3_clicked()
////////////////////////////////////////
{
  if (--m_month < 1)
  {
    m_month = 12;
    m_year--;
  }
  updateTime();
}

////////////////////////////////////////
void CMoonCal::on_pushButton_2_clicked()
////////////////////////////////////////
{
  if (++m_month > 12)
  {
    m_month = 1;
    m_year++;
  }
  updateTime();
}

////////////////////////////////////////
void CMoonCal::on_pushButton_5_clicked()
////////////////////////////////////////
{
  done(DL_OK);
}

/////////////////////////////////////////////////////////
void CMoonCal::on_comboBox_currentIndexChanged(int index)
/////////////////////////////////////////////////////////
{
  m_isMoon = index == 0;
  updateTime();
}
