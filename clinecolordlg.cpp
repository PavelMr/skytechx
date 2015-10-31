#include "clinecolordlg.h"
#include "ui_clinecolordlg.h"
#include "skcore.h"

CLineColorDlg::CLineColorDlg(QWidget *parent, QPen *pen) :
  QDialog(parent),
  ui(new Ui::CLineColorDlg)
{
  ui->setupUi(this);

  m_pen = *pen;
  ui->spinBox->setValue(m_pen.width());

  switch (pen->style())
  {
    case Qt::SolidLine:
      ui->radioButton->setChecked(true);
      break;

    case Qt::DotLine:
      ui->radioButton_2->setChecked(true);
      break;

    case Qt::DashLine:
      ui->radioButton_3->setChecked(true);
      break;

    case Qt::DashDotLine:
      ui->radioButton_4->setChecked(true);
      break;

    case Qt::DashDotDotLine:
      ui->radioButton_5->setChecked(true);
      break;

    default:
      break;
  }
}

CLineColorDlg::~CLineColorDlg()
{
  delete ui;
}

void CLineColorDlg::changeEvent(QEvent *e)
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

/////////////////////////////////////////////
void CLineColorDlg::paintEvent(QPaintEvent *)
/////////////////////////////////////////////
{
  QPainter p(this);
  QRect    rc;
  QPoint   gp = ui->groupBox->pos();
  int      m = 10;
  int      width = m_pen.width();
  QColor   col = QColor(0,0,0);

  p.setPen(QPen(QColor(col), width, Qt::SolidLine));
  rc = ui->frame->geometry().translated(gp);
  p.drawLine(rc.left() + m, rc.center().y(), rc.right() - m, rc.center().y());

  p.setPen(QPen(QColor(col), width, Qt::DotLine));
  rc = ui->frame_2->geometry().translated(gp);
  p.drawLine(rc.left() + m, rc.center().y(), rc.right() - m, rc.center().y());

  p.setPen(QPen(QColor(col), width, Qt::DashLine));
  rc = ui->frame_3->geometry().translated(gp);
  p.drawLine(rc.left() + m, rc.center().y(), rc.right() - m, rc.center().y());

  p.setPen(QPen(QColor(col), width, Qt::DashDotLine));
  rc = ui->frame_4->geometry().translated(gp);
  p.drawLine(rc.left() + m, rc.center().y(), rc.right() - m, rc.center().y());

  p.setPen(QPen(QColor(col), width, Qt::DashDotDotLine));
  rc = ui->frame_5->geometry().translated(gp);
  p.drawLine(rc.left() + m, rc.center().y(), rc.right() - m, rc.center().y());

  ui->frame_6->setStyleSheet(QString("background-color: %1").arg(m_pen.color().name()));
}

/////////////////////////////////////////////////////
void CLineColorDlg::on_spinBox_valueChanged(int arg1)
/////////////////////////////////////////////////////
{
  m_pen.setWidth(arg1);
  update();
}

///////////////////////////////////////////
void CLineColorDlg::on_pushButton_clicked()
///////////////////////////////////////////
{
  QColorDialog dlg(m_pen.color(), this);

  if (dlg.exec() == DL_OK)
  {
    m_pen.setColor(dlg.selectedColor());
    update();
  }
}

/////////////////////////////////////////////
void CLineColorDlg::on_pushButton_3_clicked()
/////////////////////////////////////////////
{
  done(DL_OK);
}

/////////////////////////////////////////////
void CLineColorDlg::on_pushButton_2_clicked()
/////////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////////////////
void CLineColorDlg::on_radioButton_toggled(bool checked)
////////////////////////////////////////////////////////
{
  if (checked)
    m_pen.setStyle(Qt::SolidLine);
}

//////////////////////////////////////////////////////////
void CLineColorDlg::on_radioButton_2_toggled(bool checked)
//////////////////////////////////////////////////////////
{
  if (checked)
    m_pen.setStyle(Qt::DotLine);
}

//////////////////////////////////////////////////////////
void CLineColorDlg::on_radioButton_3_toggled(bool checked)
//////////////////////////////////////////////////////////
{
  if (checked)
    m_pen.setStyle(Qt::DashLine);
}

//////////////////////////////////////////////////////////
void CLineColorDlg::on_radioButton_4_toggled(bool checked)
//////////////////////////////////////////////////////////
{
  if (checked)
    m_pen.setStyle(Qt::DashDotLine);
}

//////////////////////////////////////////////////////////
void CLineColorDlg::on_radioButton_5_toggled(bool checked)
//////////////////////////////////////////////////////////
{
  if (checked)
    m_pen.setStyle(Qt::DashDotDotLine);
}
