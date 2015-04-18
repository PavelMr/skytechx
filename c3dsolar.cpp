#include "c3dsolar.h"
#include "ui_c3dsolar.h"
#include "ccomdlg.h"

C3DSolar::C3DSolar(mapView_t *view, QWidget *parent, bool isComet, int index) :
  QDialog(parent),
  ui(new Ui::C3DSolar)
{
  ui->setupUi(this);
  ui->frame->setView(view, true);
  m_view = *view;
  m_jd = m_view.jd;

  setWindowFlags(Qt::Window);

  QTimer *timer = new QTimer(this);

  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
  timer->start(25);

  ui->comboBox_2->addItem(tr("None"), -1);
  ui->comboBox_2->addItem(tr("Comet"), -2);
  ui->comboBox_2->addItem(tr("Sun"), PT_SUN);
  ui->comboBox_2->addItem(tr("Mercury"), PT_MERCURY);
  ui->comboBox_2->addItem(tr("Venus"), PT_VENUS);
  ui->comboBox_2->addItem(tr("Earth"), 20);
  ui->comboBox_2->addItem(tr("Mars"), PT_MARS);
  ui->comboBox_2->addItem(tr("Jupiter"), PT_JUPITER);
  ui->comboBox_2->addItem(tr("Saturn"), PT_SATURN);
  ui->comboBox_2->addItem(tr("Uranus"), PT_URANUS);
  ui->comboBox_2->addItem(tr("Neptune"), PT_NEPTUNE);

  ui->frame->setShowHeight(ui->checkBox->isChecked());
  ui->frame->setShowEclipticPlane(ui->checkBox_2->isChecked());

  for (int i = 0; i < tComets.count(); i++)
  {
    if (tComets[i].selected)
    {
      ui->comboBox->addItem(tComets[i].name, i);
    }
  }
  updateData();

  if (isComet && index >= 0)
  {
    int idx = ui->comboBox->findData(index);
    if (idx >= 0)
    {
      ui->comboBox->setCurrentIndex(idx);
      ui->frame->generateComet(index, ui->spinBox->value(), ui->spinBox_2->value());
    }
  }
}

C3DSolar::~C3DSolar()
{
  delete ui;
}

void C3DSolar::on_pushButton_2_clicked()
{
  m_jd--;
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_3_clicked()
{
  m_jd++;
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_checkBox_toggled(bool checked)
{
  ui->frame->setShowHeight(checked);
}

void C3DSolar::on_pushButton_4_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addMonths(-1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_5_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addMonths(+1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_6_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addYears(-1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_7_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addYears(+1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_8_clicked()
{
  if (ui->comboBox->currentIndex() == -1)
  {
    return;
  }
  int index = ui->comboBox->currentData().toInt();
  ui->frame->generateComet(index, ui->spinBox->value(), ui->spinBox_2->value());
}

void C3DSolar::updateData()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);

  ui->dateEdit->blockSignals(true);
  ui->dateEdit->setDateTime(dt);
  ui->dateEdit->blockSignals(false);
  ui->label_4->setText(getStrDate(m_jd, m_view.geo.tz));
}

void C3DSolar::on_pushButton_9_clicked()
{
  ui->frame->setViewParam(0, R180);
}

void C3DSolar::on_pushButton_10_clicked()
{
  ui->frame->setViewParam(CM_UNDEF, CM_UNDEF, 0, 0, 0);
}

void C3DSolar::on_dateEdit_dateChanged(const QDate &date)
{
  QDateTime dt(date, QTime(12, 0, 0));

  m_jd = jdGetJDFrom_DateTime(&dt);
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_11_clicked()
{
  m_jd = jdGetCurrentJD();
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}


void C3DSolar::on_horizontalSlider_sliderReleased()
{
  ui->horizontalSlider->setValue(0);
}

void C3DSolar::slotTimer()
{
  QEasingCurve curve(QEasingCurve::InCubic);
  m_jd += (ui->horizontalSlider->value() > 0 ? +1 : -1) * curve.valueForProgress(qAbs(ui->horizontalSlider->value()) / 100.0) * 50.0;
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}


void C3DSolar::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void C3DSolar::on_pushButton_12_clicked()
{
  if (ui->comboBox->currentIndex() == -1)
  {
    return;
  }
  int index = ui->comboBox->currentData().toInt();

  comet_t *a = &tComets[index];

  if (a->e < 0.99)
  {
    double aa = a->q / (1.0 - a->e);
    double P = 1.00004024 * pow(aa, 1.5);
    ui->spinBox->setValue(P * 365.25 * 0.5 * 1.02);
  }
  else
  {
    msgBoxError(this, tr("Comet is not periodical!"));
  }
}

void C3DSolar::on_pushButton_13_clicked()
{
  ui->frame->removeOrbit();
}

void C3DSolar::on_checkBox_2_toggled(bool checked)
{
  ui->frame->setShowEclipticPlane(checked);
}

void C3DSolar::on_comboBox_2_currentIndexChanged(int /*index*/)
{
  int data = ui->comboBox_2->currentData().toInt();
  ui->frame->setLockAt(data);
}
