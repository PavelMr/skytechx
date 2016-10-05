#include "choreditorwidget.h"
#include "ui_choreditorwidget.h"

CHorEditorWidget::CHorEditorWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CHorEditorWidget)
{
  ui->setupUi(this);
}

CHorEditorWidget::~CHorEditorWidget()
{
  delete ui;
}
