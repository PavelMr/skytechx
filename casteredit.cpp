#include "casteredit.h"
#include "ui_casteredit.h"

CAsterEdit::CAsterEdit(QWidget *parent, bool bNew, asteroid_t *a) :
  QDialog(parent),
  ui(new Ui::CAsterEdit)
{
  ui->setupUi(this);

  if (bNew)
  {
    ui->lineEdit->setText(tr("Unnamed"));
  }
  else
  {
    ui->lineEdit->setText(a->name);
    ui->doubleSpinBox->setValue(R2D(a->M));
    ui->doubleSpinBox_2->setValue(R2D(a->peri));
    ui->doubleSpinBox_3->setValue(R2D(a->node));
    ui->doubleSpinBox_4->setValue(R2D(a->inc));
    ui->doubleSpinBox_5->setValue(a->e);
    ui->doubleSpinBox_6->setValue(R2D(a->n));
    ui->doubleSpinBox_7->setValue(a->a);
    ui->doubleSpinBox_8->setValue(a->H);
    ui->doubleSpinBox_9->setValue(a->G);

    QDateTime dt;

    jdConvertJDTo_DateTime(a->epoch, &dt);

    ui->spinBox->setValue(dt.date().year());
    ui->spinBox_2->setValue(dt.date().month());
    ui->spinBox_3->setValue(dt.date().day());
  }

  m_a = a;

}

CAsterEdit::~CAsterEdit()
{
  delete ui;
}

void CAsterEdit::changeEvent(QEvent *e)
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

////////////////////////////////////////
void CAsterEdit::on_pushButton_clicked()
////////////////////////////////////////
{
  double m = sqrt(1 + 0.00000006);
  double n = (360.0 / (365.256898326 * pow(ui->doubleSpinBox_7->value(), 1.5) / m));

  ui->doubleSpinBox_6->setValue(n);
}


////////////////////////////////////////////////////
// month change
void CAsterEdit::on_spinBox_2_valueChanged(int arg1)
////////////////////////////////////////////////////
{
  QDate d(ui->spinBox->value(), arg1, 1);

  ui->spinBox_3->setRange(1, d.daysInMonth());
}


//////////////////////////////////////////////////
// year change
void CAsterEdit::on_spinBox_valueChanged(int arg1)
//////////////////////////////////////////////////
{
  QDate d(arg1, ui->spinBox_2->value(), 1);

  ui->spinBox_3->setRange(1, d.daysInMonth());
}

//////////////////////////////////////////
// on OK
void CAsterEdit::on_pushButton_3_clicked()
//////////////////////////////////////////
{
  m_a->name = ui->lineEdit->text();
  m_a->M = D2R(ui->doubleSpinBox->value());
  m_a->peri = D2R(ui->doubleSpinBox_2->value());
  m_a->node = D2R(ui->doubleSpinBox_3->value());
  m_a->inc = D2R(ui->doubleSpinBox_4->value());
  m_a->e = ui->doubleSpinBox_5->value();
  m_a->n = D2R(ui->doubleSpinBox_6->value());
  m_a->a = ui->doubleSpinBox_7->value();
  m_a->H = ui->doubleSpinBox_8->value();
  m_a->G = ui->doubleSpinBox_9->value();
  m_a->lastJD = CM_UNDEF;

  QDateTime dt(QDate(ui->spinBox->value(), ui->spinBox_2->value(), ui->spinBox_3->value()), QTime(0,0,0));

  m_a->epoch = jdGetJDFrom_DateTime(&dt);

  done(DL_OK);
}

//////////////////////////////////////////
// on Cancel
void CAsterEdit::on_pushButton_2_clicked()
//////////////////////////////////////////
{
  done(DL_CANCEL);
}


void CAsterEdit::on_pb_copy_clicked()
{
  QString str;
  QTextStream ts(&str);

  ts << ui->lineEdit->text() << ";";
  ts << ui->spinBox->value() << ";";
  ts << ui->spinBox_2->value() << ";";
  ts << ui->spinBox_3->value() << ";";

  ts << QString::number(ui->doubleSpinBox->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_2->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_3->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_4->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_5->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_6->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_7->value(), 'f', 10) << ";";
  ts << QString::number(ui->doubleSpinBox_8->value(), 'f', 3) << ";";
  ts << QString::number(ui->doubleSpinBox_9->value(), 'f', 3);

  QApplication::clipboard()->setText(str);
}

void CAsterEdit::on_pb_from_clip_clicked()
{
  QStringList list = QApplication::clipboard()->text().split(";");

  if (list.count() != 13)
  {
    msgBoxError(this, tr("Clipboard does not contain the elements data!"));
    return;
  }

  if (msgBoxQuest(this, tr("Copy elements from clipboard?")) == QMessageBox::No)
  {
    return;
  }

  ui->lineEdit->setText(list[0]);
  ui->spinBox->setValue(list[1].toInt());
  ui->spinBox_2->setValue(list[2].toInt());
  ui->spinBox_3->setValue(list[3].toInt());

  ui->doubleSpinBox->setValue(list[4].toDouble());
  ui->doubleSpinBox_2->setValue(list[5].toDouble());
  ui->doubleSpinBox_3->setValue(list[6].toDouble());
  ui->doubleSpinBox_4->setValue(list[7].toDouble());
  ui->doubleSpinBox_5->setValue(list[8].toDouble());
  ui->doubleSpinBox_6->setValue(list[9].toDouble());
  ui->doubleSpinBox_7->setValue(list[10].toDouble());
  ui->doubleSpinBox_8->setValue(list[11].toDouble());
  ui->doubleSpinBox_9->setValue(list[12].toDouble());
}
