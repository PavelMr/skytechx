#include "csatelliteevent.h"
#include "ui_csatelliteevent.h"
#include "castro.h"
#include "csatxyz.h"

#include <QDebug>

static QColor  colors[4][8] = {{Qt::red, Qt::yellow, Qt::black, Qt::black, Qt::black, Qt::black, Qt::black, Qt::black},
                               {Qt::magenta, Qt::red, Qt::green, Qt::blue, Qt::black, Qt::black, Qt::black, Qt::black},
                               {Qt::red, Qt::yellow, Qt::blue, Qt::green, Qt::darkCyan, Qt::magenta, Qt::white, Qt::darkRed},
                               {Qt::red, Qt::yellow, Qt::black, Qt::green, Qt::magenta, Qt::white, Qt::black, Qt::black}
                              };

CSatelliteChartWidget::CSatelliteChartWidget(QWidget *parent, QLabel *label) :
  QWidget(parent),
  m_drag(false),
  m_label(label)
{
  setMouseTracking(true);
}

void CSatelliteChartWidget::setData(const mapView_t *view, int planet)
{
  m_view = *view;
  m_planet = planet;
  m_jd = m_view.jd;

  switch (planet)
  {
    case PT_MARS:
      m_xScale = 13;
      m_yScale = 1;
      break;

    case PT_JUPITER:
      m_xScale = 54;
      m_yScale = 4;
      break;

    case PT_SATURN:
      m_xScale = 82;
      m_yScale = 6;
      break;

    case PT_URANUS:
      m_xScale = 29;
      m_yScale = 10;
      break;
  }

  calculate(rect().height());

  update();
}

void CSatelliteChartWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing);

  double dy = height() / m_yScale;
  int row = 0;
  double jd = m_jd - 0.5 + m_view.geo.tz;
  double frac = 1 - ((jd) - (int)jd);
  int y = 0;
  p.setPen(Qt::black);
  p.setFont(QFont("Arial", 11, QFont::Bold));

  //qDebug() << frac << jd;

  for (double i = 0; i <= height() + dy; i += dy)
  {
    QRectF rc = QRectF(0, y, width(), dy * frac);

    if (((int)(jd) % 2) == 0)
    {
      p.fillRect(QRect(0, y - dy * (1 - frac), width(), dy), Qt::darkGray);
    }
    else
    {
      p.fillRect(QRect(0, y - dy * (1 - frac), width(), dy), Qt::gray);
    }

    if (rc.height() > 48)
    {
      double jd2 = jd + 0.5;
      p.drawText(rc.adjusted(10, 10, -10, -10), Qt::AlignLeft | Qt::AlignVCenter, getStrDate(jd2, m_view.geo.tz));
    }

    y += dy * frac;
    row++;
    jd += 1;
    frac = 1;
  }

  //p.fillRect(rect(), Qt::darkGray);

  int cx = width() / 2;
  double scale = width() / m_xScale;

  QRect planetRc = QRect(cx - scale * 0.5, 0, scale, height());
  p.fillRect(planetRc, Qt::black);
  p.setPen(Qt::white);

  for (int j = 0; j < m_satCount; j++)
  {
    int cc = 0;
    p.setPen(QPen(colors[m_planet - PT_MARS][j], 2));
    int y = 0;
    for (int i = 0; i < m_data[j].count() - 1; i++)
    {
      chart_t &p1 = m_data[j][i];
      chart_t &p2 = m_data[j][i + 1];

      int x1 = cx - p1.x * scale;
      int x2 = cx - p2.x * scale;

      cc++;

      if (p1.z < 0 && p2.z < 0)
      {
        QLine line;
        bool hidden;

        if (clipLine(x1, y, x2, y + 1, planetRc.left(), planetRc.right(), line, hidden))
        {
          p.drawLine(line);
          y++;
          continue;
        }
      }

      p.drawLine(x1, y, x2, y + 1);
      y++;
    }
  }

  //p.setPen(Qt::white);
  //p.drawText(10, 10, QString::number(m_xScale));
}

void CSatelliteChartWidget::resizeEvent(QResizeEvent *e)
{
  calculate(e->size().height());
}

void CSatelliteChartWidget::wheelEvent(QWheelEvent *e)
{
  if (e->delta() > 0)
  {
    m_xScale *= 1.2;
  }
  else
  {
    m_xScale *= 0.8;
  }

  m_xScale = CLAMP(m_xScale, 0.5, 150);

  update();
}

void CSatelliteChartWidget::mousePressEvent(QMouseEvent *e)
{
  m_drag = true;
  m_lastPoint = e->pos();
}

void CSatelliteChartWidget::mouseMoveEvent(QMouseEvent *e)
{
  if (m_drag)
  {
    m_jd += (m_lastPoint.y() - e->pos().y()) * 0.005 * m_yScale;
    calculate(height());
    m_lastPoint = e->pos();
    update();
  }


  if (m_satCount)
  {
    double jd = m_data[0][e->pos().y()].jd;
    m_label->setText(getStrDate(jd, m_view.geo.tz) + " " + getStrTime(jd, m_view.geo.tz));
  }
}

void CSatelliteChartWidget::mouseReleaseEvent(QMouseEvent *)
{
  m_drag = false;
}

void CSatelliteChartWidget::calculate(int height)
{
  double step = m_yScale / (double)height;
  double from = m_jd;

  m_satCount = 0;

  for (int i = 0; i < 8; i++)
  {
    m_data[i].clear();
  }

  if (step == 0)
  {
    return;
  }

  m_step = step;

  for (double jd = from; jd < from + m_yScale; jd += step)
  {
    satxyz_t sat;
    orbit_t o;
    orbit_t s;

    m_view.jd = jd;

    cAstro.setParam(&m_view);
    cAstro.calcPlanet(m_planet, &o);
    cAstro.calcPlanet(PT_SUN, &s);

    if (cSatXYZ.solve(jd, m_planet, &o, &s, &sat))
    {
      m_satCount = sat.count;
      for (int i = 0; i < sat.count; i++)
      {
        chart_t data;

        data.jd = jd;
        data.x = sat.sat[i].x;
        data.z = sat.sat[i].z;
        m_data[i].append(data);
      }
    }
  }
}


CSatelliteEvent::CSatelliteEvent(QWidget *parent, const mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CSatelliteEvent)
{
  m_view = *view;
  ui->setupUi(this);
  CAstro astro;

  m_chart = new CSatelliteChartWidget(this, ui->label_dt);
  ui->verticalLayout->addWidget(m_chart);

  ui->comboBox->addItem(astro.getName(PT_MARS), PT_MARS);
  ui->comboBox->addItem(astro.getName(PT_JUPITER), PT_JUPITER);
  ui->comboBox->addItem(astro.getName(PT_SATURN), PT_SATURN);
  ui->comboBox->addItem(astro.getName(PT_URANUS), PT_URANUS);
}

CSatelliteEvent::~CSatelliteEvent()
{
  delete ui;
}

void CSatelliteEvent::on_comboBox_currentIndexChanged(int index)
{
  m_chart->setData(&m_view, ui->comboBox->itemData(index).toInt());

  satxyz_t sat;
  orbit_t o;
  orbit_t s;

  cAstro.setParam(&m_view);
  cAstro.calcPlanet(index + PT_MARS, &o);
  cAstro.calcPlanet(PT_SUN, &s);

  if (cSatXYZ.solve(m_view.jd, index + PT_MARS, &o, &s, &sat))
  {
    QString temp = "<font color='%1'>%2</font>";
    QLabel *label[8] = {ui->label, ui->label_2, ui->label_3, ui->label_4,
                        ui->label_5, ui->label_6, ui->label_7, ui->label_8};

    for (int i = 0; i < 8; i++)
    {
      label[i]->setText("");
    }

    for (int i = 0; i < sat.count; i++)
    {
      label[i]->setText(temp.arg(colors[index][i].name(), sat.sat[i].name));
    }
  }
}
