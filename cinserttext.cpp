#include "skcore.h"
#include "cinserttext.h"
#include "ui_cinserttext.h"
#include "cfontcolordlg.h"

static QFont lastFont("Arial", 14);

CInsertText::CInsertText(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CInsertText)
{
  m_font = lastFont;
  ui->setupUi(this);

  ui->radioButton->setChecked(true);
}

CInsertText::~CInsertText()
{
  delete ui;
}

/////////////////////////////////////////
void CInsertText::on_pushButton_clicked()
/////////////////////////////////////////
{
  CFontColorDlg dlg(this, &m_font);

  if (dlg.exec() == DL_OK)
  {
    m_font = dlg.m_font;
  }
}

///////////////////////////////////////////
void CInsertText::on_pushButton_3_clicked()
///////////////////////////////////////////
{
  m_text = ui->lineEdit->text();
  m_bRect = ui->checkBox->isChecked();

  if (ui->radioButton->isChecked())
    m_align = 0;
  else
  if (ui->radioButton_2->isChecked())
    m_align = 1;
  else
  if (ui->radioButton_3->isChecked())
    m_align = 2;
  else
  if (ui->radioButton_4->isChecked())
    m_align = 3;
  else
  if (ui->radioButton_5->isChecked())
    m_align = 4;

  if (m_text.isEmpty())
    return;

  lastFont = m_font;
  done(DL_OK);
}

///////////////////////////////////////////
void CInsertText::on_pushButton_2_clicked()
///////////////////////////////////////////
{
  done(DL_CANCEL);
}
