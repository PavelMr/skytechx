#include "csatelliteevent.h"
#include "ui_csatelliteevent.h"
#include "castro.h"
#include "csatxyz.h"

#include <QDebug>

static QColor  colors[10] = {Qt::red, Qt::yellow, Qt::blue, Qt::green, Qt::darkCyan, Qt::magenta, Qt::white, Qt::darkRed, Qt::darkBlue, Qt::cyan};
static bool    invert = false;
static int     planetIndex = 0;

CSatelliteChartWidget::CSatelliteChartWidget(QWidget *parent, QLabel *label) :
  QWidget(parent),
  m_drag(false),
  m_label(label)
{
  setMouseTracking(true);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
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

    case PT_NEPTUNE:
      m_xScale = 59;
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

  double jd2 = (m_jd + m_view.geo.tz) - 0.5;
  double dy = height() / m_yScale;
  double y = -(jd2 - (floor(jd2))) * dy;
  int mod = (int)(jd2);

  p.setPen(Qt::black);
  p.setFont(QFont("Arial", 11, QFont::Bold));

  for (double i = 0; i <= height() + dy; i += dy)
  {
    QRectF rc = QRectF(0, y, width(), dy);

    if ((mod % 2) == 0)
    {
      p.fillRect(rc, Qt::darkGray);
    }
    else
    {
      p.fillRect(rc, Qt::gray);
    }

    if (rc.height() > 35)
    {
      p.drawText(rc.adjusted(10, 10, -10, -10), Qt::AlignLeft | Qt::AlignVCenter, getStrDate(jd2 + 0.5, 0));
    }

    mod++;
    y += dy;
    jd2++;
  }

  int cx = width() / 2;
  double scale = width() / m_xScale;

  QRect planetRc = QRect(cx - scale * 0.5, 0, scale, height());
  if (planetRc.width() < 1) planetRc.setWidth(1);
  p.fillRect(planetRc, Qt::black);
  p.setPen(Qt::white);

  for (int j = 0; j < m_satCount; j++)
  {
    int cc = 0;
    int y = 0;
    p.setPen(QPen(colors[j], 2));
    for (int i = 0; i < m_data[j].count() - 1; i++)
    {
      chart_t &p1 = m_data[j][i];
      chart_t &p2 = m_data[j][i + 1];

      int x1;
      int x2;

      if (!invert)
      {
        x1 = cx - p1.x * scale;
        x2 = cx - p2.x * scale;
      }
      else
      {
        x1 = cx + p1.x * scale;
        x2 = cx + p2.x * scale;
      }

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

  m_xScale = CLAMP(m_xScale, 0.5, 1000);
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
    if (rect().contains(e->pos()))
    {
      double jd = m_data[0][e->pos().y()].jd;
      m_label->setText(getStrDate(jd, m_view.geo.tz) + " " + getStrTime(jd, m_view.geo.tz));
    }
    else
    {
      m_label->setText(tr("N/A"));
    }
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
    CPlanetSatellite planSat;
    planetSatellites_t sats;
    orbit_t o;
    orbit_t s;

    m_view.jd = jd;

    cAstro.setParam(&m_view);
    cAstro.calcPlanet(m_planet, &o);
    cAstro.calcPlanet(PT_SUN, &s);

    planSat.solve(m_view.jd - o.light, m_planet, &sats, &o, &s, false);
    m_satCount = sats.sats.count();
    for (int i = 0; i < m_satCount; i++)
    {
      chart_t data;

      data.jd = jd;
      data.x = sats.sats[i].ex;
      data.z = sats.sats[i].ez;
      m_data[i].append(data);
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
  ui->comboBox->addItem(astro.getName(PT_NEPTUNE), PT_NEPTUNE);

  ui->comboBox->setCurrentIndex(planetIndex);
  ui->checkBox->setChecked(invert);
}

CSatelliteEvent::~CSatelliteEvent()
{
  invert = ui->checkBox->isChecked();
  planetIndex = ui->comboBox->currentIndex();
  delete ui;
}

void CSatelliteEvent::on_comboBox_currentIndexChanged(int index)
{
  m_chart->setData(&m_view, ui->comboBox->itemData(index).toInt());

  CPlanetSatellite planSat;
  planetSatellites_t sats;
  orbit_t o;
  orbit_t s;

  cAstro.setParam(&m_view);
  cAstro.calcPlanet(index + PT_MARS, &o);
  cAstro.calcPlanet(PT_SUN, &s);

  planSat.solve(m_view.jd - o.light, index + PT_MARS, &sats, &o, &s, false);
  QString temp = "<font color='%1'>%2</font>";
  QLabel *label[10] = {ui->label, ui->label_2, ui->label_3, ui->label_4,
                       ui->label_5, ui->label_6, ui->label_7, ui->label_8,
                       ui->label_9, ui->label_10};

  for (int i = 0; i < 10; i++)
  {
    label[i]->setText("");
  }

  for (int i = 0; i < sats.sats.count(); i++)
  {
    label[i]->setText(temp.arg(colors[i].name(), sats.sats[i].name));
  }
}

void CSatelliteEvent::on_pushButton_3_clicked()
{
  done(DL_OK);
}

void CSatelliteEvent::on_checkBox_toggled(bool checked)
{
  invert = checked;
  m_chart->update();
}
