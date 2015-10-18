#include "moonlessnightsdlg.h"
#include "ui_moonlessnightsdlg.h"
#include "skutils.h"

MoonlessNightsDlg::MoonlessNightsDlg(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::MoonlessNightsDlg)
{
  ui->setupUi(this);
  m_view = *view;
  m_startJD = (floor(view->jd + 0.5) - 0.5) - view->geo.tz;
  fillList();

  connect(ui->widget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pushButton_2_clicked()));
}

MoonlessNightsDlg::~MoonlessNightsDlg()
{
  delete ui;
}

void MoonlessNightsDlg::fillList()
{
  CAstro ast;
  double jd = m_startJD;

  ui->widget->removeAll();

  for (int i = 0; i < 50; i++)
  {
    orbit_t moon;

    m_view.jd = jd + i;
    ast.setParam(&m_view);
    ast.calcPlanet(PT_MOON, &moon);

    if ((moon.phase * 100.) < ui->spinBox->value())
    {
      ui->widget->addRow(getStrDate(m_view.jd, m_view.geo.tz) + " " + getStrTime(m_view.jd, m_view.geo.tz), m_view.jd);
    }
  }
}

void MoonlessNightsDlg::on_pushButton_3_clicked()
{
  fillList();
}

void MoonlessNightsDlg::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void MoonlessNightsDlg::on_pushButton_2_clicked()
{
  int index = ui->widget->getSelectionIndex();
  if (index == -1)
  {
    return;
  }
  m_jd = ui->widget->getCustomData(index).toDouble();
  done(DL_OK);
}
