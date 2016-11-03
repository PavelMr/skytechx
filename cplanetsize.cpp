#include "cplanetsize.h"
#include "ui_cplanetsize.h"
#include "castro.h"
#include "cskpainter.h"
#include "transform.h"
#include "cplanetrenderer.h"

#include <QPainter>
#include <QVBoxLayout>

static mapView_t *mw;
static double diameter = 0;

double FOV;
#define FOV_MIN   D2R(0.1)
#define FOV_MAX   D2R(0.005)

CPlanetSize::CPlanetSize(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CPlanetSize)
{
  ui->setupUi(this);

  QVBoxLayout *layout = new QVBoxLayout(ui->widget);
  m_widget = new CWidget(NULL);
  layout->addWidget(m_widget);
  layout->setMargin(0);

  m_view = *view;
  mw = &m_view;

  FOV = D2R(0.05);

  QTimer *timer = new QTimer(this);
  timer->start(10);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
  updateLabels();

  QDesktopWidget widget;
  QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());
  resize(mainScreenSize.width() * 0.8, 400);
}

CPlanetSize::~CPlanetSize()
{
  delete ui;
}

CWidget::CWidget(QWidget *parent) : QWidget(parent)
{
  setCursor(Qt::CrossCursor);
  setMouseTracking(true);
}

void CWidget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  int w = rect().width();
  int h = rect().height();

  QImage     img(w, h, QImage::Format_ARGB32);

  img.fill(Qt::black);
  CSkPainter p(&img);
  p.setRenderHint(QPainter::Antialiasing);

  p.setPen(QPen(Qt::white, 1, Qt::DotLine));
  p.drawLine(0, h / 2, w, h / 2);

  SKPOINT pt;

  pt.sx = 0;
  pt.sy = h / 2.;
  int space = 8;

  for (int i = PT_MERCURY; i <= PT_NEPTUNE; i++)
  {
    orbit_t o, s;
    mapView_t view = *mw;
    CAstro cAstro;

    cAstro.setParam(&view);
    cAstro.calcPlanet(i, &o);
    cAstro.calcPlanet(PT_SUN, &s);
    view.roll = 0;
    view.flipX = 0;
    view.flipY = 0;
    view.fov = FOV;

    trfCreateMatrixView(&cAstro, &view, w, h);

    if (i == PT_MERCURY)
    {
      pt.sx += space * 2;
    }
    pt.sx += trfGetArcSecToPix(o.sx) * (i == PT_SATURN ? 2.5 : 1) + space;

    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    cPlanetRenderer.renderPlanet(&pt, &o, &s, &view, &p, &img, 0, true);

    pt.sx += trfGetArcSecToPix(o.sx) * (i == PT_SATURN ? 2.5 : 1) + space;
  }  

  painter.drawImage(0, 0, blurredImage(img, 2, false));
  painter.setOpacity(0.25);
  painter.drawImage(0, 0, img);
}

void CWidget::mouseMoveEvent(QMouseEvent *event)
{
  double h2 = height() / 2.;
  diameter = 2 * (qAbs(event->pos().y() - h2) / width()) * FOV;
}

void CWidget::wheelEvent(QWheelEvent *e)
{
  if (e->delta() > 0)
  {
    FOV *= 0.8;
  }
  else
  {
    FOV *= 1.2;
  }

  FOV = CLAMP(FOV, FOV_MAX, FOV_MIN);

  update();
}

void CPlanetSize::on_pushButton_2_clicked()
{
  mw->jd = jdGetCurrentJD();
  updateLabels();
  update();
}

void CPlanetSize::on_horizontalSlider_sliderReleased()
{
  ui->horizontalSlider->setValue(0);
}

void CPlanetSize::slotTimer()
{
  QEasingCurve curve(QEasingCurve::InCubic);

  if (ui->horizontalSlider->value() == 0)
  {
    updateLabels();
    return;
  }

  mw->jd += (ui->horizontalSlider->value() > 0 ? +1 : -1) * curve.valueForProgress(qAbs(ui->horizontalSlider->value()) / 100.0) * 50.0;
  update();
  updateLabels();
}

void CPlanetSize::updateLabels()
{
  ui->label_3->setText(getStrDate(mw->jd, mw->geo.tz) + " / " + getStrTime(mw->jd, mw->geo.tz));
  ui->label_4->setText(tr("Diameter : ") + getStrDegNoSign(diameter));
}


void CPlanetSize::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CPlanetSize::on_pushButton_3_clicked()
{
  m_jd = mw->jd;
  done(DL_OK);
}
