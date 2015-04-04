#include "skcore.h"
#include "cinserttext.h"
#include "ui_cinserttext.h"
#include "cfontcolordlg.h"
#include "cskpainter.h"

static QFont lastFont;
static QString lastText = "";
static bool firstTime = true;

CInsertText::CInsertText(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CInsertText)
{
  ui->setupUi(this);

  if (firstTime)
  {
    lastFont.setFamily("Arial");
    lastFont.setPixelSize(18);
    firstTime = false;
  }

  m_font = lastFont;
  ui->lineEdit->setText(lastText);
  ui->lineEdit->selectAll();

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
    m_align = RT_TOP_LEFT;
  else
  if (ui->radioButton_2->isChecked())
    m_align = RT_BOTTOM_LEFT;
  else
  if (ui->radioButton_3->isChecked())
    m_align = RT_BOTTOM_RIGHT;
  else
  if (ui->radioButton_4->isChecked())
    m_align = RT_TOP_RIGHT;
  else
  if (ui->radioButton_5->isChecked())
    m_align = RT_CENTER;

  if (m_text.isEmpty())
    return;

  lastFont = m_font;
  lastText = m_text;

  done(DL_OK);
}

///////////////////////////////////////////
void CInsertText::on_pushButton_2_clicked()
///////////////////////////////////////////
{
  done(DL_CANCEL);
}
