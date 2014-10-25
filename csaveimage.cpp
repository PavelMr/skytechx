#include "csaveimage.h"
#include "ui_csaveimage.h"

#include "skcore.h"

static int radioButton = 0;
static bool keepAspectRatio = true;
static int quality = 80;

CSaveImage::CSaveImage(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSaveImage)
{
  ui->setupUi(this);

  switch (radioButton)
  {
    case 0:
      ui->radioButton->setChecked(true);
      ui->radioButton_2->setChecked(false);
      break;

    case 1:
      ui->radioButton->setChecked(false);
      ui->radioButton_2->setChecked(true);
      break;
  }

  ui->checkBox->setChecked(keepAspectRatio);
  ui->horizontalSlider->setValue(quality);
  setQuality();
}

void CSaveImage::setSize(int w, int h)
{
  m_aspectRatio = w / (double)h;

  ui->spinBox->setValue(w);
  ui->spinBox_2->setValue(h);
}

void CSaveImage::getSize(int &w, int &h, int &quality)
{
  w = ui->spinBox->value();
  h = ui->spinBox_2->value();
  quality = ui->horizontalSlider->value();
}

CSaveImage::~CSaveImage()
{
  delete ui;
}

void CSaveImage::on_pushButton_2_clicked()
{
  if (ui->radioButton->isChecked())
  {
    radioButton = 0;
  }
  else
  {
    radioButton = 1;
  }

  keepAspectRatio = ui->checkBox->isChecked();

  done(DL_OK);
}

void CSaveImage::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CSaveImage::on_radioButton_toggled(bool checked)
{
  ui->checkBox->setEnabled(!checked);
  ui->spinBox->setEnabled(!checked);
  ui->spinBox_2->setEnabled(!checked);
}

void CSaveImage::on_radioButton_2_toggled(bool checked)
{
  ui->checkBox->setEnabled(checked);
  ui->spinBox->setEnabled(checked);
  ui->spinBox_2->setEnabled(checked);
}

void CSaveImage::on_spinBox_valueChanged(int arg1)
{
  if (!ui->checkBox->isChecked())
  { // no aspect ratio
    return;
  }

  ui->spinBox_2->blockSignals(true);
  ui->spinBox_2->setValue(arg1 / m_aspectRatio);
  ui->spinBox_2->blockSignals(false);
}

void CSaveImage::on_spinBox_2_valueChanged(int arg1)
{
  if (!ui->checkBox->isChecked())
  { // no aspect ratio
    return;
  }

  ui->spinBox->blockSignals(true);
  ui->spinBox->setValue(arg1 * m_aspectRatio);
  ui->spinBox->blockSignals(false);
}

void CSaveImage::on_checkBox_toggled(bool checked)
{
  if (checked)
  {
    ui->spinBox->blockSignals(true);
    ui->spinBox_2->blockSignals(true);
    ui->spinBox_2->setValue(ui->spinBox->value() / m_aspectRatio);
    ui->spinBox->blockSignals(false);
    ui->spinBox_2->blockSignals(false);
  }
}

void CSaveImage::on_horizontalSlider_valueChanged(int)
{
  setQuality();
}

void CSaveImage::setQuality()
{
  quality = ui->horizontalSlider->value();
  ui->label_quality->setText(QString(tr("JPG Quality : %1")).arg(quality));
}
