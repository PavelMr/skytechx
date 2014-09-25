#include "ctimelapsepref.h"
#include "ui_ctimelapsepref.h"
#include "skcore.h"
#include "castro.h"

static int updateEveryMS = DEFAULT_TIME_LAPSE_UPDATE;
static int stepX = 1;
static int comboIndex;

CTimeLapsePref::CTimeLapsePref(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CTimeLapsePref)
{
  ui->setupUi(this);

  ui->comboBox->addItem(tr("Seconds"));
  ui->comboBox->addItem(tr("Minutes"));
  ui->comboBox->addItem(tr("Hours"));
  ui->comboBox->addItem(tr("Days"));
  ui->comboBox->setCurrentIndex(comboIndex);

  ui->spinBox->setValue(updateEveryMS);
  ui->spinBox_2->setValue(stepX);
}

CTimeLapsePref::~CTimeLapsePref()
{
  delete ui;
}

void CTimeLapsePref::changeEvent(QEvent *e)
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

void CTimeLapsePref::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void CTimeLapsePref::on_pushButton_clicked()
{
  updateEveryMS = m_updateSpeed = ui->spinBox->value();
  stepX = m_multiplicator = ui->spinBox_2->value();

  comboIndex = ui->comboBox->currentIndex();
  switch (comboIndex)
  {
    case 0: break;
    case 1: m_multiplicator *= 60; break;
    case 2: m_multiplicator *= 3600; break;
    case 3: m_multiplicator *= 3600 * 24; m_multiplicator /= SID_DAY; break;
    default: Q_ASSERT(false);
  }

  done(DL_OK);
}
