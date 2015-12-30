#include "csetposition.h"
#include "cmapview.h"
#include "ui_csetposition.h"
#include "castro.h"

CSetPosition::CSetPosition(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSetPosition)
{
  ui->setupUi(this);
  ui->horizontalSlider->setRange(1, (int)R2D(MAX_MAP_FOV));
}

///////////////////////////////////////////////////////////////////
void CSetPosition::init(double x, double y, double fov, double rot)
///////////////////////////////////////////////////////////////////
{
  m_x = x;
  m_y = y;
  m_fov = fov;
  m_roll = rot;

  ui->doubleSpinBox->setValue(R2D(rot));

  ui->horizontalSlider->setValue(ceil(R2D(fov)));

  int v1, v2, v3;

  // ra/dec
  getHMSFromRad(x / 15., &v1, &v2, &v3);
  ui->spinBox->setValue(v1);
  ui->spinBox_2->setValue(v2);
  ui->spinBox_3->setValue(v3);

  getDMSFromRad(fabs(y), &v1, &v2, &v3);
  ui->spinBox_5->setValue(v1 * (y >= 0 ? 1 : -1));
  ui->spinBox_4->setValue(v2);
  ui->spinBox_6->setValue(v3);

  // alt/azm
  double alt, azm;

  cAstro.convRD2AARef(x, y, &azm, &alt);

  getDMSFromRad(azm, &v1, &v2, &v3);
  ui->spinBox_7->setValue(v1);
  ui->spinBox_8->setValue(v2);
  ui->spinBox_9->setValue(v3);

  getDMSFromRad(fabs(alt), &v1, &v2, &v3);
  ui->spinBox_11->setValue(v1 * (alt >= 0 ? 1 : -1));
  ui->spinBox_10->setValue(v2);
  ui->spinBox_12->setValue(v3);

  // ecl
  double lon, lat;

  cAstro.convRD2Ecl(x, y, &lon, &lat);

  getDMSFromRad(lon, &v1, &v2, &v3);
  ui->spinBox_13->setValue(v1);
  ui->spinBox_14->setValue(v2);
  ui->spinBox_15->setValue(v3);

  getDMSFromRad(fabs(lat), &v1, &v2, &v3);
  ui->spinBox_17->setValue(v1 * (lat >= 0 ? 1 : -1));
  ui->spinBox_16->setValue(v2);
  ui->spinBox_18->setValue(v3);
}

CSetPosition::~CSetPosition()
{
  delete ui;
}

void CSetPosition::changeEvent(QEvent *e)
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

//////////////////////////////////////////////////////////////
void CSetPosition::on_horizontalSlider_valueChanged(int value)
//////////////////////////////////////////////////////////////
{
  double val = D2R((double)value);

  ui->label->setText(tr("FOV : ") + getStrDeg(val));
}


//////////////////////////
void CSetPosition::check()
//////////////////////////
{
  if (ui->spinBox_5->value() == 90 || ui->spinBox_5->value() == -90)
  {
    ui->spinBox_4->setValue(0);
    ui->spinBox_6->setValue(0);
  }

  if (ui->spinBox_11->value() == 90 || ui->spinBox_11->value() == -90)
  {
    ui->spinBox_10->setValue(0);
    ui->spinBox_12->setValue(0);
  }
}


//////////////////////////////////////////
// on OK
void CSetPosition::on_pushButton_clicked()
//////////////////////////////////////////
{
  double ra, dec;

  if (ui->tabWidget->currentIndex() == 0)
  { // RA/DEC
    ra = HMS2RAD(ui->spinBox->value(), ui->spinBox_2->value(), ui->spinBox_3->value());
    dec = DMS2RAD(abs(ui->spinBox_5->value()), ui->spinBox_4->value(), ui->spinBox_6->value());
    if (ui->spinBox_5->value() < 0)
      dec = -dec;

    m_x = ra;
    m_y = dec;
  }
  else
  if (ui->tabWidget->currentIndex() == 1)
  { // Alt/Azm
    double alt, azm;

    azm = DMS2RAD(ui->spinBox_7->value(), ui->spinBox_8->value(), ui->spinBox_9->value());
    alt = DMS2RAD(abs(ui->spinBox_11->value()), ui->spinBox_10->value(), ui->spinBox_12->value());
    if (ui->spinBox_11->value() < 0)
      alt = -alt;

    cAstro.convAA2RDRef(azm, alt, &ra, &dec);

    m_x = ra;
    m_y = dec;
  }
  else
  if (ui->tabWidget->currentIndex() == 2)
  { // ecl.
    double lon, lat;

    lon = DMS2RAD(ui->spinBox_13->value(), ui->spinBox_14->value(), ui->spinBox_15->value());
    lat = DMS2RAD(abs(ui->spinBox_17->value()), ui->spinBox_16->value(), ui->spinBox_18->value());
    if (ui->spinBox_17->value() < 0)
      lat = -lat;

    cAstro.convEcl2RD(lon, lat, &ra, &dec);

    m_x = ra;
    m_y = dec;
  }

  if (ui->checkBox->isChecked())
    m_fov = CM_UNDEF;
  else
    m_fov = D2R(ui->horizontalSlider->value());

  m_roll = D2R(ui->doubleSpinBox->value());

  done(DL_OK);
}

void CSetPosition::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void CSetPosition::on_checkBox_toggled(bool checked)
{
  ui->horizontalSlider->setEnabled(!checked);
}
