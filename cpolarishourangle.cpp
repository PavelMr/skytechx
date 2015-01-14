#include "cpolarishourangle.h"
#include "ui_cpolarishourangle.h"
#include "precess.h"
#include "castro.h"
#include "skcore.h"
#include "cskpainter.h"

#include <QDebug>

CPolarisHourAngle::CPolarisHourAngle(QWidget *parent, const mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CPolarisHourAngle),
  m_polarisHourAngle(0)
{
  ui->setupUi(this);
  setFixedHeight(size().height());

  m_reticle = new QPixmap(":/res/reticle.png");
  m_view = *view;

  updateTime();
}

CPolarisHourAngle::~CPolarisHourAngle()
{
  delete ui;
}

void CPolarisHourAngle::changeEvent(QEvent *e)
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

void CPolarisHourAngle::paintEvent(QPaintEvent *)
{
  CSkPainter p(this);
  QPointF center = ui->frame->rect().center();
  double r1 = 175;
  double r2 = 160;

  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);

  p.drawPixmap(ui->frame->pos(), *m_reticle);
  p.setPen(Qt::yellow);
  p.setBrush(Qt::white);
  p.translate(ui->frame->pos());

  double angle = m_polarisHourAngle * MPI2 + MPI;
  double s = sin(angle);
  double c = cos(angle);

  p.drawLine(QPoint(255, 255), QPoint(center.x() + s * r1, center.y() + c * r1));
  p.drawEllipse(QPoint(center.x() + s * r2, center.y() + c * r2), 5, 5);
  p.drawEllipse(QPoint(center.x(), center.y()), 5, 5);
}

void CPolarisHourAngle::updateTime()
{
  CAstro ast;

  ast.setParam(&m_view);
  m_polarisHourAngle = ast.getPolarisHourAngle();

  ui->labelDate->setText(getStrDate(m_view.jd, m_view.geo.tz));
  ui->labelTime->setText(getStrTime(m_view.jd, m_view.geo.tz));
  ui->labelPolarisHA->setText(getStrTime(m_polarisHourAngle, 0, false, true));

  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd + m_view.geo.tz, &dt);

  ui->dateTimeEdit->blockSignals(true);
  ui->dateTimeEdit->setDateTime(dt);
  ui->dateTimeEdit->blockSignals(false);

  update();
}

void CPolarisHourAngle::on_pushButton_clicked()
{
  m_view.jd = jdGetCurrentJD();
  updateTime();
}

void CPolarisHourAngle::on_pushButton_2_clicked()
{
  done(DL_OK);
}

void CPolarisHourAngle::on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
  QDateTime dt = QDateTime(dateTime);

  m_view.jd = jdGetJDFrom_DateTime(&dt) - m_view.geo.tz;
  updateTime();
}
