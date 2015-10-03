#include "cwposmap.h"
#include "ui_cwposmap.h"
#include "skcore.h"
#include "cskpainter.h"

CWPosMap::CWPosMap(QWidget *parent, double lon, double lat, QList<QPointF> &list) :
  QDialog(parent),
  ui(new Ui::CWPosMap)
{
  ui->setupUi(this);
  setFixedSize(size());
  //m_pix = new QPixmap("../data/maps/earth_day.jpg");
  m_pix = new QPixmap("../data/maps/earthmap4k.jpg");
  m_lon = CM_UNDEF;
  m_oldLon = lon;
  m_oldLat = lat;
  m_list = list;

  ui->widget->setOldLonLat(m_oldLon, m_oldLat);
  ui->widget->setPixmap(m_pix);
  ui->widget->setList(list);

  connect(ui->widget, SIGNAL(sigLonLatChange(double,double)), this, SLOT(slotLonLatChanged(double,double)));
  connect(ui->widget, SIGNAL(sigSiteChange(double,double)), this, SLOT(slotSiteChanged(double,double)));
}

CWPosMap::~CWPosMap()
{
  delete ui;
}

void CWPosMap::changeEvent(QEvent *e)
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


//////////////////////////////////////
void CWPosMap::on_pushButton_clicked()
//////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////
void CWPosMap::on_pushButton_2_clicked()
////////////////////////////////////////
{
  if (m_lon == CM_UNDEF)
  {
    msgBoxError(this, tr("No location selected!"));
    return;
  }

  done(DL_OK);
}

void CWPosMap::slotLonLatChanged(double lon, double lat)
{
  ui->label->setText(getStrLon(D2R(lon)) + "   " + getStrLat(D2R(lat)));
}

void CWPosMap::slotSiteChanged(double lon, double lat)
{
  m_lon = lon;
  m_lat = lat;
}

void CWPosMap::on_pushButton_3_clicked()
{
  ui->widget->resetView();
}
