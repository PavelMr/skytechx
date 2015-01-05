#include "csatellitesearch.h"
#include "ui_csatellitesearch.h"
#include "skcore.h"
#include "csgp4.h"
#include "castro.h"

CSatelliteSearch::CSatelliteSearch(mapView_t *view, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSatelliteSearch)
{
  ui->setupUi(this);
  m_mapView = view;

  for (int i = 0; i < sgp4.count(); i++)
  {
    ui->listView->addRow(sgp4.getName(i), i);
  }
}

CSatelliteSearch::~CSatelliteSearch()
{
  delete ui;
}

void CSatelliteSearch::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void CSatelliteSearch::on_listView_doubleClicked(const QModelIndex &)
{
  on_pushButton_clicked();
}

void CSatelliteSearch::on_pushButton_clicked()
{
  int index = ui->listView->getCustomData(ui->listView->getSelectionIndex()).toInt();

  radec_t rd;
  satellite_t s;

  if (!sgp4.solve(index, m_mapView, &s))
  {
    msgBoxError(this, "Error - Cannot compute!!!");
    return;
  }

  cAstro.setParam(m_mapView);
  cAstro.convAA2RDRef(s.azimuth, s.elevation, &rd.Ra, &rd.Dec);

  qDebug() << index << s.elevation << s.azimuth;

  m_ra = rd.Ra;
  m_dec = rd.Dec;
  m_fov = getOptObjFov(0, 0, D2R(2.5));

  done(DL_OK);
}
