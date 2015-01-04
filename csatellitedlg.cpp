#include "csatellitedlg.h"
#include "ui_csatellitedlg.h"

CSatelliteDlg::CSatelliteDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSatelliteDlg)
{
  ui->setupUi(this);
}

CSatelliteDlg::~CSatelliteDlg()
{
  delete ui;
}
