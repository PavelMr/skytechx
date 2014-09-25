#include "csavetm.h"
#include "ui_csavetm.h"

CSaveTM::CSaveTM(QWidget *parent, double jd) :
  QDialog(parent),
  ui(new Ui::CSaveTM)
{
  ui->setupUi(this);
  setFixedSize(size());
  ui->lineEdit->setFocus();
  m_jd = jd;

  ui->label->setText(getStrDate(jd, 0) + " " + getStrTime(jd, 0));
}

CSaveTM::~CSaveTM()
{
  delete ui;
}

void CSaveTM::changeEvent(QEvent *e)
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

// OK
void CSaveTM::on_pushButton_2_clicked()
{
  QString str = ui->lineEdit->text();

  if (str.isEmpty() || str.contains(';'))
  {
    ui->lineEdit->setFocus();
    return;
  }

  SkFile f("data/timemarks/timemarks.dat");
  QTextStream s(&f);

  if (f.open(SkFile::WriteOnly | SkFile::Text | SkFile::Append))
  {
    s << str << ";" << QString::number(m_jd, 'f', 8) << "\n";
  }

  done(DL_OK);
}

// cancel
void CSaveTM::on_pushButton_clicked()
{
  done(DL_CANCEL);
}
