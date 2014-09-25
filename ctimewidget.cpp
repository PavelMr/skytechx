#include "ctimewidget.h"
#include "ui_ctimewidget.h"

#include "jd.h"
#include "castro.h"

CTimeWidget::CTimeWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CTimeWidget)
{
  ui->setupUi(this);
}

CTimeWidget::~CTimeWidget()
{
  delete ui;
}

void CTimeWidget::changeEvent(QEvent *e)
{
  QWidget::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void CTimeWidget::timeUpdate(mapView_t *view)
{
  if (isHidden())
    return;

  CAstro ast;

  double jd = jdGetCurrentJD();
  mapView_t v = *view;

  v.jd = jd;
  ast.setParam(&v);

  ui->label_t1->setText(getStrTime(jd, v.geo.tz));
  ui->label_t2->setText(getStrTime(jd, 0));
  ui->label_t3->setText(getStrTime(ast.m_lst / M_PI / 2 + 0.5, 0, false, true));
  ui->label_t7->setText(getStrTime(ast.getPolarisHourAngle(), 0, false, true));
  ui->label_t5->setText(QString("%1").arg(jd, 0, 'f', 5));
  ui->label_t6->setText(QString("%1").arg(ast.m_deltaT * 60 * 60 * 24, 0, 'f', 2) + tr(" sec."));
}
