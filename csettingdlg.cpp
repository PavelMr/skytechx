#include "csettingdlg.h"
#include "ui_csettingdlg.h"

/*
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
*/

CSettingDlg::CSettingDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSettingDlg)
{
  ui->setupUi(this);

  m_bkGradient = new QPixmap(ui->label_10->width(), ui->label_10->height());
  makeBkGrad();

  ui->checkBox_3->setChecked(g_skSet.map.background.bStatic);
  ui->checkBox_4->setChecked(!g_skSet.map.background.bStatic);

  ui->frame_16->setStyleSheet("background : " + QColor(g_skSet.map.background.staticColor).name());

  ui->frame->setValues(g_skSet.map.starRange, 10);
  ui->spinBox->setValue(R2D(g_skSet.map.star.propNamesFromFov));
  ui->spinBox_3->setValue(R2D(g_skSet.map.star.bayerFromFov));
  ui->spinBox_4->setValue(R2D(g_skSet.map.star.flamsFromFov));

  ui->checkBox->setChecked(g_skSet.map.star.bayerPriority);

  setFrameStyle(ui->frame_12, FONT_STAR_PNAME);

  ////////////////////


}

CSettingDlg::~CSettingDlg()
{
  delete m_bkGradient;
  delete ui;
}

void CSettingDlg::changeEvent(QEvent *e)
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

//////////////////////////////
void CSettingDlg::makeBkGrad()
//////////////////////////////
{
  QPainter p;
  QLinearGradient g(QPoint(0, 0), QPoint(m_bkGradient->width() - 1, m_bkGradient->height()));

  g.setColorAt(0, g_skSet.map.background.dynamicColor[0]);
  g.setColorAt(0.5f, g_skSet.map.background.dynamicColor[1]);
  g.setColorAt(1, g_skSet.map.background.dynamicColor[2]);

  p.begin(m_bkGradient);
  p.fillRect(m_bkGradient->rect(), g);
  p.end();

  ui->label_10->setPixmap(*m_bkGradient);
}

//////////////////////////////////////////////////////
void CSettingDlg::setFrameStyle(QFrame *frm, int font)
//////////////////////////////////////////////////////
{
  frm->setStyleSheet("background : " + QColor(g_skSet.fonst[font].color).name());
}

// nigth color /////////////////////////////
void CSettingDlg::on_pushButton_21_clicked()
////////////////////////////////////////////
{
  QColorDialog dlg(this);

  dlg.setCurrentColor(g_skSet.map.background.dynamicColor[0]);

  if (dlg.exec() == DL_OK)
  {
    g_skSet.map.background.dynamicColor[0] = dlg.selectedColor().toRgb().rgba();
    makeBkGrad();
  }
}

// twilight color /////////////////////////
void CSettingDlg::on_pushButton_23_clicked()
////////////////////////////////////////////
{
  QColorDialog dlg(this);

  dlg.setCurrentColor(g_skSet.map.background.dynamicColor[1]);

  if (dlg.exec() == DL_OK)
  {
    g_skSet.map.background.dynamicColor[1] = dlg.selectedColor().toRgb().rgba();
    makeBkGrad();
  }
}

// day color ///////////////////////////////
void CSettingDlg::on_pushButton_22_clicked()
////////////////////////////////////////////
{
  QColorDialog dlg(this);

  dlg.setCurrentColor(g_skSet.map.background.dynamicColor[2]);

  if (dlg.exec() == DL_OK)
  {
    g_skSet.map.background.dynamicColor[2] = dlg.selectedColor().toRgb().rgba();
    makeBkGrad();
  }
}

/////////////////////////////////////////////////////
void CSettingDlg::on_checkBox_3_toggled(bool checked)
/////////////////////////////////////////////////////
{
  g_skSet.map.background.bStatic = checked;
  ui->checkBox_4->setChecked(!g_skSet.map.background.bStatic);
}

/////////////////////////////////////////////////////
void CSettingDlg::on_checkBox_4_toggled(bool checked)
/////////////////////////////////////////////////////
{
  g_skSet.map.background.bStatic = !checked;
  ui->checkBox_3->setChecked(g_skSet.map.background.bStatic);
}

////////////////////////////////////////////
void CSettingDlg::on_pushButton_20_clicked()
////////////////////////////////////////////
{
  QColorDialog dlg(this);

  dlg.setCurrentColor(g_skSet.map.background.staticColor);

  if (dlg.exec() == DL_OK)
  {
    g_skSet.map.background.staticColor = dlg.selectedColor().toRgb().rgba();
    ui->frame_16->setStyleSheet("background : " + QColor(g_skSet.map.background.staticColor).name());
  }
}
