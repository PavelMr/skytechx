#include "cplanetvis.h"
#include "ui_cplanetvis.h"

CPlanetVis::CPlanetVis(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CPlanetVis)
{
  ui->setupUi(this);
  setFixedSize(size());

  m_view = *view;
  updateTitle();

  QColor col = QColor(32, 32, 32);
  ui->frame_12->setStyleSheet("background:" + col.name());
  col = QColor(200, 32, 32);
  ui->frame_13->setStyleSheet("background:" + col.name());
  col = QColor(32, 200, 32);
  ui->frame_14->setStyleSheet("background:" + col.name());
}

CPlanetVis::~CPlanetVis()
{
  delete ui;
}

void CPlanetVis::changeEvent(QEvent *e)
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

//////////////////////////////////////////
void CPlanetVis::paintEvent(QPaintEvent *)
//////////////////////////////////////////
{
  QPainter p(this);
  int     id[9] = {PT_MERCURY,
                   PT_VENUS,
                   PT_MARS,
                   PT_JUPITER,
                   PT_SATURN,
                   PT_URANUS,
                   PT_NEPTUNE,
                   PT_MOON,
                   PT_SUN
                  };

  QFrame *frm[9] = {ui->frame,
                    ui->frame_2,
                    ui->frame_3,
                    ui->frame_4,
                    ui->frame_5,
                    ui->frame_6,
                    ui->frame_7,
                    ui->frame_8,
                    ui->frame_9,
                   };

  QRect fRect;
  QRect rc;
  float x = 0;
  float delta;

  fRect = ui->frame_10->geometry();

  delta = fRect.width() / 24.0;

  for (int h = 0; h < 24; h++)
  {
    rc = fRect;
    rc.setX(x + fRect.x());
    rc.setWidth(delta);

    p.setPen(Qt::gray);
    p.setBrush(Qt::NoBrush);
    p.drawRect(rc);
    p.setPen(Qt::black);
    p.drawText(rc, Qt::AlignCenter, QString::number(h));

    x += delta;
  }

  for (int i = 0; i < 9; i++)
  {
    QRect     fRect;
    QRect     rc;
    float     x = 0;
    float     delta;
    orbit_t   orbit;
    orbit_t   sun;
    CAstro    astro;
    mapView_t view = m_view;
    double    jd = (floor(m_view.jd + 0.5) - 0.5) - m_view.geo.tz;

    fRect = frm[i]->geometry();

    delta = fRect.width() / 24.0;

    p.setPen(QPen(Qt::gray, 1, Qt::DotLine));
    for (int h = 0; h < 24; h++)
    {
      view.jd = jd;
      astro.setParam(&view);

      astro.calcPlanet(PT_SUN, &sun);
      astro.calcPlanet(id[i], &orbit);

      rc = fRect;
      rc.setX(x + fRect.x());
      rc.setWidth(delta);

      QColor col;

      if (orbit.lAlt <= 0)
        col = QColor(32, 32, 32);
      else
      {
        if (sun.lAlt >= 0)
          col = QColor(200, 32, 32);
        else
          col = QColor(32, 200, 32);
      }

      p.fillRect(rc, col);
      p.drawLine(rc.right(), rc.top() + 1, rc.right(), rc.bottom() - 1);

      jd += 1 / 24.0;

      x += delta;
    }
  }
}

void CPlanetVis::updateTitle()
{
  setWindowTitle(tr("Planet visibility at ") + getStrDate(m_view.jd, m_view.geo.tz));
}


////////////////////////////////////////
void CPlanetVis::on_pushButton_clicked()
////////////////////////////////////////
{
  done(DL_OK);
}

void CPlanetVis::on_pushButton_2_clicked()
{
  m_view.jd += 1;
  updateTitle();
  update();
}

void CPlanetVis::on_pushButton_3_clicked()
{
  m_view.jd -= 1;
  updateTitle();
  update();
}

void CPlanetVis::on_pushButton_4_clicked()
{
  m_view.jd = jdGetCurrentJD();
  updateTitle();
  update();
}
