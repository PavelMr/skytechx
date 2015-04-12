#include "c3dsolar.h"
#include "ui_c3dsolar.h"
#include "ccomdlg.h"

C3DSolar::C3DSolar(mapView_t *view, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::C3DSolar)
{
  ui->setupUi(this);
  ui->frame->setView(view, true);
  m_view = *view;
  m_jd = m_view.jd;

  setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
  showMaximized();

  for (int i = 0; i < tComets.count(); i++)
  {
    if (tComets[i].selected)
    {
      ui->comboBox->addItem(tComets[i].name, i);
    }
  }
  updateData();
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
  int index = ui->comboBox->currentData().toInt();
  ui->frame->generateComet(index, ui->spinBox->value(), ui->spinBox_2->value());
}

void C3DSolar::updateData()
{
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
