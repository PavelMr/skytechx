#include "ctextsel.h"
#include "ui_ctextsel.h"

CTextSel::CTextSel(QWidget *parent, QString title, int maxChars, QString defText) :
  QDialog(parent),
  ui(new Ui::CTextSel)
{
  ui->setupUi(this);
  setFixedSize(size());
  setWindowTitle(title);
  ui->lineEdit->setMaxLength(maxChars);
  ui->lineEdit->setText(defText);
  ui->lineEdit->selectAll();
}

CTextSel::~CTextSel()
{
  delete ui;
}

void CTextSel::changeEvent(QEvent *e)
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

// on OK
void CTextSel::on_pushButton_2_clicked()
{
  m_text = ui->lineEdit->text();

  if (m_text.isEmpty())
  {
    ui->lineEdit->setFocus();
    return;
  }

  done(DL_OK);
}

// on Cancel
void CTextSel::on_pushButton_clicked()
{
  done(DL_CANCEL);
}
