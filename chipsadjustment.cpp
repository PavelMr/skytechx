#include "chipsadjustment.h"
#include "cimagemanip.h"
#include "hipsmanager.h"
#include "hipsrenderer.h"
#include "ui_chipsadjustment.h"

extern imageParam_t hipsImageParam;

CHIPSAdjustment::CHIPSAdjustment(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CHIPSAdjustment)
{
  ui->setupUi(this);
  setParams();
}

CHIPSAdjustment::~CHIPSAdjustment()
{
  delete ui;
}

void CHIPSAdjustment::setParams()
{
  ui->horizontalSlider->setValue(hipsImageParam.brightness);
  ui->horizontalSlider_2->setValue(hipsImageParam.contrast);
  ui->horizontalSlider_3->setValue(hipsImageParam.gamma);
  ui->checkBox->setChecked(hipsImageParam.invert);
}

void CHIPSAdjustment::on_pushButton_clicked()
{
  hipsImageParam.brightness = ui->horizontalSlider->value();
  hipsImageParam.contrast = ui->horizontalSlider_2->value();
  hipsImageParam.gamma = ui->horizontalSlider_3->value();
  hipsImageParam.invert = ui->checkBox->isChecked();

  g_hipsRenderer->manager()->getCache()->clear();
  pcMapView->repaintMap();
}

void CHIPSAdjustment::on_pushButton_2_clicked()
{
  g_hipsRenderer->manager()->resetImageParams();
  setParams();
  on_pushButton_clicked();
}
