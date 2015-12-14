#include "cplanetaltitude.h"
#include "ui_cplanetaltitude.h"
#include "castro.h"
#include "skcore.h"

CPlanetAltitude::CPlanetAltitude(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CPlanetAltitude)
{
  ui->setupUi(this);
  CAstro ast;

  for (int p = 0; p < PT_PLANET_COUNT; p++)
  {
    ui->comboBox->addItem(ast.getName(p));
  }

  ui->comboBox->setCurrentIndex(0);

  m_view = *view;
  m_jd = view->jd;
  calculate(m_jd);
}

CPlanetAltitude::~CPlanetAltitude()
{
  delete ui;
}

void CPlanetAltitude::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  int height = ui->frame->height();
  int width = ui->frame->width();
  int count = m_list[0].count();
  double deltax = width / (double)(count - 1);

  p.translate(ui->frame->pos());
  p.setClipRect(0, 0, width, height);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);

  QPixmap img(count, 1);
  QPainter imgp;

  imgp.begin(&img);

  for (int j = 0; j < count; j++)
  {
    int c;
    double d = m_list[PT_SUN].at(j);

    if (d < DEG2RAD(-18))
      c = 0;
    else
    if (d < DEG2RAD(-12))
      c = 32;
    else
    if (d < DEG2RAD(-6))
      c = 64;
    else
    if (d < DEG2RAD(0))
      c = 86;
    else
      c = 100;

    imgp.setPen(QColor(c, c, c));
    imgp.drawLine(j, 0, j, 1);
  }

  imgp.end();

  p.drawPixmap(QRect(0, 0, width, height), img);
  p.setPen(QPen(Qt::white, 1, Qt::DotLine));
  p.drawLine(0, height / 2, width, height / 2);
  p.drawLine(0, height / 4, width, height / 4);
  p.drawLine(0, height / 2 + height / 4, width, height / 2 + height / 4);

  int i = ui->comboBox->currentIndex();
  {
    for (int j = 0; j < count - 1; j++)
    {
      double alt1 = R2D(m_list[i].at(j));
      double alt2 = R2D(m_list[i].at(j + 1));

      int    x1 = j * deltax;
      int    x2 = x1 + deltax;
      double a1 = (height / 180.0) * (180 - (alt1 + 90));
      double a2 = (height / 180.0) * (180 - (alt2 + 90));

      p.setPen(Qt::white);
      p.drawLine(x1, a1, x2, a2);
    }
  }

  for (int i = 1; i < 24; i++)
  {
    int x = ((i / 24.0) * width);

    p.setOpacity(1);
    p.setPen(Qt::white);
    p.drawLine(x, height, x, height - 8);

    p.drawText(x - 10, height - 8 - 15, 20, 12, Qt::AlignCenter, QString("%1").arg(qAbs((i  + 12) % 24)));

    p.setOpacity(0.3);
    p.setPen(QPen(Qt::white, 1, Qt::DotLine));
    p.drawLine(x, height - 22, x, 0);
  }

  p.setOpacity(0.5);
  p.fillRect(0, height / 2.0 , width, height / 2.0, Qt::black);
  p.setOpacity(1);

  p.drawText(0, height / 4, 20, 20, Qt::AlignCenter, "45°");
  p.drawText(0, height / 2, 20, 20, Qt::AlignCenter, "0°");
  p.drawText(0, height / 2 + height / 4, 20, 20, Qt::AlignCenter, "-45°");


}

void CPlanetAltitude::calculate(double jd)
{
  m_step = JD1SEC * 60 * 30; // 10 min
  CAstro ast;

  for (int p = 0; p < PT_PLANET_COUNT; p++)
  {
    m_list[p].clear();
  }

  jd = (floor(jd + 0.5)) - m_view.geo.tz;

  for (double i = 0; i <= 1 + m_step; i += m_step)
  {
    m_view.jd = jd + i;
    ast.setParam(&m_view);
    for (int p = 0; p < PT_PLANET_COUNT; p++)
    {
      orbit_t o;

      ast.calcPlanet(p, &o);

      m_list[p].append(o.lAlt);
    }
  }

  setWindowTitle(tr("Planet altitude") + " " + getStrDate(jd, m_view.geo.tz));
}

void CPlanetAltitude::on_comboBox_currentIndexChanged(int)
{
  update();
}

void CPlanetAltitude::on_pushButton_clicked()
{
  done(DL_OK);
}

void CPlanetAltitude::on_pushButton_2_clicked()
{
  m_jd--;
  calculate(m_jd);
  update();
}

void CPlanetAltitude::on_pushButton_3_clicked()
{
  m_jd++;
  calculate(m_jd);
  update();
}

void CPlanetAltitude::on_pushButton_4_clicked()
{
  m_jd = jdGetCurrentJD();
  calculate(m_jd);
  update();
}
