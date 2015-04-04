#include "cfontcolordlg.h"
#include "ui_cfontcolordlg.h"
#include "skcore.h"

CFontColorDlg::CFontColorDlg(QWidget *parent, QFont *font, bool bSetColor, QColor color) :
  QDialog(parent),
  ui(new Ui::CFontColorDlg)
{
  ui->setupUi(this);

  qDebug() << font->pixelSize();

  ui->fontComboBox->setCurrentFont(*font);
  ui->spinBox->setValue(font->pixelSize());
  ui->checkBox->setChecked(font->bold());
  ui->checkBox_2->setChecked(font->italic());

  m_font = *font;

  if (!bSetColor)
  {
    ui->label_3->setDisabled(true);
    ui->pushButton->setDisabled(true);
    m_setColor = false;
  }
  else
  {
    m_setColor = true;
    m_color = color;
  }

  updateExample();
}

CFontColorDlg::~CFontColorDlg()
{
  delete ui;
}

void CFontColorDlg::changeEvent(QEvent *e)
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

///////////////////////////////////
void CFontColorDlg::updateExample()
///////////////////////////////////
{
  if (m_setColor)
  {
    QPalette pal(m_color);
    QColor bk = pal.windowText().color();

    ui->label_2->setStyleSheet(QString("color: %1; background-color: %2").arg(m_color.name()).arg(bk.name()));
    ui->label_3->setStyleSheet(QString("background-color: %1").arg(m_color.name()));
  }

  ui->label_2->setFont(m_font);
}


//////////////////////////////////////////////////////
void CFontColorDlg::on_checkBox_stateChanged(int arg1)
//////////////////////////////////////////////////////
{
  m_font.setBold((bool)arg1);

  updateExample();
}

////////////////////////////////////////////////////////
void CFontColorDlg::on_checkBox_2_stateChanged(int arg1)
////////////////////////////////////////////////////////
{
  m_font.setItalic((bool)arg1);

  updateExample();
}

//////////////////////////////////////////////////////////////////////
void CFontColorDlg::on_fontComboBox_currentFontChanged(const QFont &f)
//////////////////////////////////////////////////////////////////////
{
  m_font.setFamily(f.family());

  updateExample();
}

/////////////////////////////////////////////////////
void CFontColorDlg::on_spinBox_valueChanged(int arg1)
/////////////////////////////////////////////////////
{
  m_font.setPixelSize(arg1);

  updateExample();
}


///////////////////////////////////////////
void CFontColorDlg::on_pushButton_clicked()
///////////////////////////////////////////
{
  QColorDialog dlg(m_color, this);

  if (dlg.exec() == DL_OK)
  {
    m_color = dlg.currentColor();
    updateExample();
  }
}

void CFontColorDlg::on_pushButton_3_clicked()
{
  done(DL_OK);
}

void CFontColorDlg::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}
