#include "cplnsearch.h"
#include "ui_cplnsearch.h"
#include "skcore.h"
#include "castro.h"

CPlnSearch::CPlnSearch(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CPlnSearch)
{
  ui->setupUi(this);
  //setFixedSize(size());

  ui->widget->addRow(cAstro.getName(PT_SUN), PT_SUN);
  ui->widget->addRow(cAstro.getName(PT_MOON), PT_MOON);
  ui->widget->addRow(cAstro.getName(PT_MERCURY), PT_MERCURY);
  ui->widget->addRow(cAstro.getName(PT_VENUS), PT_VENUS);
  ui->widget->addRow(cAstro.getName(PT_MARS), PT_MARS);
  ui->widget->addRow(cAstro.getName(PT_JUPITER), PT_JUPITER);
  ui->widget->addRow(cAstro.getName(PT_SATURN), PT_SATURN);
  ui->widget->addRow(cAstro.getName(PT_URANUS), PT_URANUS);
  ui->widget->addRow(cAstro.getName(PT_NEPTUNE), PT_NEPTUNE);

  connect(ui->widget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDblClk(QModelIndex)));
}

CPlnSearch::~CPlnSearch()
{
  delete ui;
}

void CPlnSearch::on_pushButton_2_clicked()
{
  int i = ui->widget->getSelectionIndex();

  if (i == -1)
    return;

  int p = ui->widget->getCustomData(i).toInt();

  orbit_t o;

  cAstro.calcPlanet(p, &o);

  m_ra = o.lRD.Ra;
  m_dec = o.lRD.Dec;
  m_fov = getOptObjFov(o.sx / 3600., o.sy / 3600.);

  done(DL_OK);
}

void CPlnSearch::findPlanet(int id, mapView_t *view, double &ra, double &dec, double &fov)
{
  orbit_t o;
  CAstro ast;

  ast.setParam(view);

  ast.calcPlanet(id, &o);

  ra = o.lRD.Ra;
  dec = o.lRD.Dec;
  fov = getOptObjFov(o.sx / 3600., o.sy / 3600.);
}

void CPlnSearch::slotDblClk(QModelIndex)
{
  on_pushButton_2_clicked();
}

void CPlnSearch::on_pushButton_clicked()
{
  done(DL_CANCEL);
}
