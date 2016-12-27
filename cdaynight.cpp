#include "cdaynight.h"
#include "ui_cdaynight.h"
#include "jd.h"
#include "castro.h"
#include "cskpainter.h"

////////////////////////////////////////
CDayNight::CDayNight(QWidget *parent, mapView_t *view) :
  QDialog(parent),
////////////////////////////////////////
  ui(new Ui::CDayNight)
{
  ui->setupUi(this);
  setFixedSize(size());

  m_view = *view;

  m_jd = m_view.jd;

  m_day = new QImage("../data/maps/earth_day.jpg");
  m_night = new QImage("../data/maps/earth_night.jpg");
  m_img = new QImage(m_day->width(), m_day->height(), QImage::Format_ARGB32_Premultiplied);
  m_shade = new QImage(SHD_X, SHD_Y, QImage::Format_ARGB32_Premultiplied);

  updateMap();
}

///////////////////////
CDayNight::~CDayNight()
///////////////////////
{
  delete m_day;
  delete m_night;
  delete m_img;
  delete m_shade;

  delete ui;
}

//////////////////////////////////////
void CDayNight::changeEvent(QEvent *e)
//////////////////////////////////////
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

/////////////////////////////////////////
void CDayNight::paintEvent(QPaintEvent *)
/////////////////////////////////////////
{
  QPainter p(this);
  QRect    rc;

  rc.setTopLeft(ui->frame->pos() + ui->groupBox->pos());
  rc.setSize(ui->frame->size());

  p.drawImage(rc, *m_img);

  rc.setTopLeft(ui->frame_2->pos());
  rc.setSize(ui->frame_2->size());
  rc.adjust(1, 1, -1, -1);

  QRect    rc2;

  rc2 = rc;
  rc2.setTop(rc.top() + 32);
  rc2.setBottom(rc.bottom());

  p.setPen(Qt::darkGray);
  p.setBrush(Qt::NoBrush);
  p.drawRect(rc);
  p.drawLine(rc2.left(), rc2.top(), rc2.right(), rc2.top());

  for (int i = 1; i < 24; i++)
  {
    int x = rc2.x() + ((i / 24.0) * rc2.width());

    p.setPen(Qt::darkGray);
    p.drawLine(x, rc2.top(), x, rc2.top() + 5);

    p.setPen(Qt::black);
    p.drawText(x - 10, rc2.top(), 20, rc2.height(), Qt::AlignCenter, QString("%1").arg(i));
  }

  rc2.setTop(rc.top());
  rc2.setBottom(rc.top() + 32);

  rc2.adjust(1, 1, 0, -1);

  double jdStep = 1 / 24.0;
  mapView_t    view;
  CAstro       ast;
  orbit_t      sun;

  view = m_view;  
  double jd = getStartOfDay(m_jd, view.geo.tz);

  QImage img(24, 1, QImage::Format_ARGB32);
  img.fill(Qt::blue);

  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  for (int i = 0; i < 24; i++)
  {
    view.jd = jd;
    ast.setParam(&view);
    ast.sunEphemerid_Fast(&sun);
    ast.calcPlanet(PT_SUN, &sun);

    int c;
    double d = sun.lAlt;

    if (d < DEG2RAD(-18))
      c = 0;
    else
    if (d < DEG2RAD(-12))
      c = 100;
    else
    if (d < DEG2RAD(-6))
      c = 150;
    else
    if (d < DEG2RAD(0))
      c = 200;
    else
      c = 250;

    img.setPixel(i, 0, QColor(c, c, c).rgba());
    jd += jdStep;
  }

  p.drawImage(rc2, img);
}


///////////////////////////////////////////////////////////////////////////////
double CDayNight::getDistance(double lon, double lat, double lon2, double lat2)
///////////////////////////////////////////////////////////////////////////////
{
  double dLat = (lat2 - lat);
  double dLon = (lon2 - lon);
  double a = POW2(sin(dLat / 2)) +
                  cos((lat)) * cos((lat2)) *
                  POW2(sin(dLon / 2));
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return(c);
}


///////////////////////////
void CDayNight::updateMap()
///////////////////////////
{
  orbit_t   o;
  CAstro    a;

  if (m_jd > MAX_JD)
    m_jd = MAX_JD;
  else
  if (m_jd < MIN_JD)
    m_jd = MIN_JD;

  m_view.jd = m_jd;

  ui->label->setText(getStrTime(m_view.jd, m_view.geo.tz) + "\n" + getStrDate(m_view.jd, m_view.geo.tz));

  a.setParam(&m_view);
  a.calcPlanet(PT_SUN, &o);

  double UT = ((m_jd - (int)m_jd) + 0.5) * MPI2;
  double GST = o.hLon + UT + R180;

  double sLon = o.gRD.Ra - GST;
  double sLat = o.gRD.Dec;

  rangeDbl(&sLon, R360);

  for (int y = 0; y < SHD_Y; y++)
  {
    QRgb *data = (QRgb *)m_shade->bits() + y * SHD_X;
    double lat = y * (180 / (double)SHD_Y) - 90.0;
    for (int x = 0; x < SHD_X; x++)
    {
      double lon = x * (360 / (double)SHD_X) - 180.0;

      double d = getDistance(sLon, sLat, DEG2RAD(lon), DEG2RAD(-lat));
      int    i;

      if (d > DEG2RAD(90 + 18))
        i = 255;      
      else
      if (d > DEG2RAD(90 + 12))
        i = 180;
      else
      if (d > DEG2RAD(90 + 6))
        i = 120;
      else
      if (d > DEG2RAD(90))
        i = 64;      
      else
        i = 0;

      *data = QColor(0, 0, 0, i).rgba();
      data++;
    }
  }

  QImage b;
  if (ui->checkBox->isChecked())
  {
    b = blurredImage(*m_shade, 6, true);
  }
  else
  {
    b = *m_shade;
  }

  float qx = 0;
  float qy = 0;

  float addx = SHD_X / (float)m_img->width();
  float addy = SHD_Y / (float)m_img->height();

  for (int y = 0; y < m_img->height(); y++)
  {
    const QRgb *pb = (QRgb *)b.bits() + (int)qy * b.width();
    QRgb *data = (QRgb *)m_img->bits() + y * m_img->width();
    const QRgb *day = (QRgb *)m_day->bits() + y * m_day->width();
    const QRgb *night = (QRgb *)m_night->bits() + y * m_night->width();
    qx = 0;
    for (int x = 0; x < m_img->width(); x++)
    {
      QColor n = *night;
      QColor d = *day;

      float f = ((*(pb + (int)qx) >> 24) & 0xff) / 256.f;

      *data = QColor(LERP(f, d.red(), n.red()),
                     LERP(f, d.green(), n.green()),
                     LERP(f, d.blue(), n.blue())).rgb();

      night++;
      day++;
      data++;

      qx += addx;
    }
    qy += addy;
  }

  CSkPainter p;

  p.begin(m_img);

  p.setRenderHint(QPainter::Antialiasing, true);

  // draw sun
  double lo = sLon + R180;
  rangeDbl(&lo, R360);
  int sx = RAD2DEG(lo) / 360.0 * m_img->width();
  int sy = RAD2DEG(-sLat + R90) / 180.0 * m_img->height();

  p.setPen(QColor(200, 200, 0));
  p.setBrush(QColor(255, 255, 0));
  p.drawCross(sx, sy, 10);
  p.drawCrossX(sx, sy, 10);
  p.drawEllipse(QPoint(sx, sy), 5, 5);

  // draw cur obs. pos.
  sx = RAD2DEG(m_view.geo.lon + R180) / 360.0 * m_img->width();
  sy = RAD2DEG(-m_view.geo.lat + R90) / 180.0 * m_img->height();

  p.setPen(QColor(255, 0, 0));
  p.drawCross(sx, sy, 8);

  p.end();

  update();
}


///////////////////////////////////////
void CDayNight::on_pushButton_clicked()
///////////////////////////////////////
{
  m_jd -= 1;
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_3_clicked()
/////////////////////////////////////////
{
  m_jd += 1;
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_8_clicked()
/////////////////////////////////////////
{
  m_jd -= 1 / 24.0;
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_9_clicked()
/////////////////////////////////////////
{
  m_jd += 1 / 24.0;
  updateMap();
}

//////////////////////////////////////////
void CDayNight::on_pushButton_10_clicked()
//////////////////////////////////////////
{
  m_jd -= 1 / 24.0 / 60.0;
  updateMap();
}

//////////////////////////////////////////
void CDayNight::on_pushButton_11_clicked()
//////////////////////////////////////////
{
  m_jd += 1 / 24.0 / 60.0;
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_4_clicked()
/////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addMonths(-1);
  m_jd = jdGetJDFrom_DateTime(&dt);
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_5_clicked()
/////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addMonths(1);
  m_jd = jdGetJDFrom_DateTime(&dt);
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_6_clicked()
/////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addYears(-1);
  m_jd = jdGetJDFrom_DateTime(&dt);
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_7_clicked()
/////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addYears(1);
  m_jd = jdGetJDFrom_DateTime(&dt);
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_12_clicked()
/////////////////////////////////////////
{
  m_jd = jdGetCurrentJD();
  updateMap();
}

/////////////////////////////////////////
void CDayNight::on_pushButton_2_clicked()
/////////////////////////////////////////
{
  done(DL_CANCEL);
}

void CDayNight::on_pushButton_13_clicked()
{
  done(DL_OK);
}

void CDayNight::on_checkBox_toggled(bool checked)
{
  updateMap();
}
