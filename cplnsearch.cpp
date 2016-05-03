#include "cplnsearch.h"
#include "ui_cplnsearch.h"
#include "skcore.h"
#include "castro.h"

CPlnSearch::CPlnSearch(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CPlnSearch)
{
  ui->setupUi(this);

  ui->widget->addRow(cAstro.getName(PT_SUN), PT_SUN);
  ui->widget->addRow(cAstro.getName(PT_MOON), PT_MOON);
  ui->widget->addRow(cAstro.getName(PT_MERCURY), PT_MERCURY);
  ui->widget->addRow(cAstro.getName(PT_VENUS), PT_VENUS);
  ui->widget->addRow(cAstro.getName(PT_MARS), PT_MARS);
  ui->widget->addRow(cAstro.getName(PT_JUPITER), PT_JUPITER);
  ui->widget->addRow(cAstro.getName(PT_SATURN), PT_SATURN);
  ui->widget->addRow(cAstro.getName(PT_URANUS), PT_URANUS);
  ui->widget->addRow(cAstro.getName(PT_NEPTUNE), PT_NEPTUNE);
  ui->widget->addRow(cAstro.getName(PT_EARTH_SHADOW), PT_EARTH_SHADOW);

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

  orbit_t o, m;

  if (p == PT_EARTH_SHADOW)
  {
    cAstro.calcPlanet(PT_MOON, &m);
    cAstro.calcEarthShadow(&o, &m);

    m_ra = o.lRD.Ra;
    m_dec = o.lRD.Dec;
    m_fov = getOptObjFov(o.sx / 3600., o.sy / 3600.);    

    m_mapObj.type = MO_EARTH_SHD;

    done(DL_OK);
    return;
  }

  cAstro.calcPlanet(p, &o);

  m_ra = o.lRD.Ra;
  m_dec = o.lRD.Dec;
  m_fov = getOptObjFov(o.sx / 3600., o.sy / 3600.);

  m_mapObj.type = MO_PLANET;
  m_mapObj.par1 = p;
  m_mapObj.par2 = 0;

  done(DL_OK);
}

void CPlnSearch::findPlanet(int id, mapView_t *view, double &ra, double &dec, double &fov)
{
  orbit_t o, m;
  CAstro ast;

  ast.setParam(view);

  if (id == PT_EARTH_SHADOW)
  {
    cAstro.calcPlanet(PT_MOON, &m);
    cAstro.calcEarthShadow(&o, &m);

    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    fov = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    return;
  }

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
