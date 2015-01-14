#include "cdeltat.h"
#include "ui_cdeltat.h"
#include "castro.h"

static double lastDelta = 0;

CDeltaT::CDeltaT(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CDeltaT)
{
  ui->setupUi(this);

  m_view = *view;

  ui->comboBox->blockSignals(true);
  ui->comboBox->addItem("Table (0..2013) (Out off table use E&M (2006))");
  ui->comboBox->addItem("Morrison & Stephenson (1982)");
  ui->comboBox->addItem("Espenak (1987, 1989)");
  ui->comboBox->addItem("Espenak & Meeus (2006)*");
  ui->comboBox->addItem("IAU (1952)");
  ui->comboBox->blockSignals(false);
  ui->comboBox->setCurrentIndex(m_view.deltaTAlg);

  if (view->deltaT != CM_UNDEF)
    ui->doubleSpinBox->setValue(view->deltaT * 60 * 60 * 24);
  else
    ui->doubleSpinBox->setValue(lastDelta);

  if (view->deltaT == CM_UNDEF)
    ui->checkBox->setChecked(false);
  else
    ui->checkBox->setChecked(true);

  setValues();
}

CDeltaT::~CDeltaT()
{
  delete ui;
}

/////////////////////////////
void CDeltaT::setValues(void)
/////////////////////////////
{
  CAstro a;

  m_view.deltaTAlg = ui->comboBox->currentIndex();
  if (ui->comboBox->isEnabled())
    m_view.deltaT = CM_UNDEF;
  else
    m_view.deltaT = ui->doubleSpinBox->value() / 24.0 / 60.0 / 60.0;
  a.setParam(&m_view);

  double comp =  a.m_deltaT * 60 * 60 * 24;
  QString str = QString(tr("Use another DeltaT. Current value is %1 sec.")).arg(comp, 0, 'f', 1);

  ui->checkBox->setText(str);

  if (ui->checkBox->isChecked())
  {
    ui->doubleSpinBox->setEnabled(true);
    ui->comboBox->setEnabled(false);
  }
  else
  {
    ui->doubleSpinBox->setEnabled(false);
    ui->comboBox->setEnabled(true);
  }
}


////////////////////////////////////
void CDeltaT::changeEvent(QEvent *e)
////////////////////////////////////
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

/////////////////////////////////////
// on OK
void CDeltaT::on_pushButton_clicked()
/////////////////////////////////////
{
  if (ui->checkBox->isChecked())
  {
    m_delta = ui->doubleSpinBox->value() / 24.0 / 60.0 / 60.0;
  }
  else
  {
    m_delta = CM_UNDEF;
  }
  lastDelta = ui->doubleSpinBox->value();
  m_delta_alg = ui->comboBox->currentIndex();

  done(DL_OK);
}

///////////////////////////////////
void CDeltaT::on_checkBox_clicked()
///////////////////////////////////
{
  setValues();
}

///////////////////////////////////////
// on Cancel
void CDeltaT::on_pushButton_2_clicked()
///////////////////////////////////////
{
  done(DL_CANCEL);
}

//////////////////////////////////////////////////
void CDeltaT::on_comboBox_currentIndexChanged(int)
//////////////////////////////////////////////////
{
  setValues();
}

///////////////////////////////////////
void CDeltaT::on_checkBox_toggled(bool)
///////////////////////////////////////
{
  setValues();
}

///////////////////////////////////////////////////
void CDeltaT::on_doubleSpinBox_valueChanged(double)
///////////////////////////////////////////////////
{
  setValues();
}
