#include "cskeventdocdialog.h"
#include "ui_cskeventdocdialog.h"

#include "skcore.h"

#include <QPrinter>
#include <QPrintDialog>

AspectRatioPixmapLabel::AspectRatioPixmapLabel(QWidget *parent) :
    QFrame(parent)
{
}

void AspectRatioPixmapLabel::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  if (m_pix.isNull())
  {
    return;
  }
  QPixmap pix = m_pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.drawPixmap(width() / 2 - pix.width() / 2, height() / 2 - pix.height() / 2, pix);
}

void AspectRatioPixmapLabel::setPixmap(const QPixmap &p)
{
  m_pix = p;
  update();
}

CSkEventDocDialog::CSkEventDocDialog(QWidget *parent, SkEventDocument *event) :
  QDialog(parent),
  ui(new Ui::CSkEventDocDialog)
{
  setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
  m_event = event;

  QDesktopWidget dsk;

  ui->setupUi(this);
  resize(dsk.height() * 0.8 / 1.414, dsk.height() * 0.8);

  AspectRatioPixmapLabel *label = new AspectRatioPixmapLabel();
  m_label = label;
  ui->verticalLayout->addWidget(label);
  updateGeometry();

  QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect();

  label->setGraphicsEffect(eff);
  label->setMinimumSize(512, 512);
  label->setFrameStyle(QFrame::Box);
}

CSkEventDocDialog::~CSkEventDocDialog()
{
  delete ui;
}

void CSkEventDocDialog::resizeEvent(QResizeEvent *)
{
  QImage image(getSize(), QImage::Format_ARGB32);
  m_event->create(&image, ui->checkBox->isChecked());

  QPixmap pixmap = QPixmap::fromImage(image);
  m_label->setPixmap(pixmap);
}

void CSkEventDocDialog::on_pushButton_clicked()
{
  done(DL_OK);
}

void CSkEventDocDialog::on_pushButton_2_clicked()
{
  QPrinter prn(QPrinter::ScreenResolution);
  QPrintDialog dlg(&prn, this);

  prn.setOrientation(QPrinter::Portrait);
  prn.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

  if (dlg.exec() == DL_CANCEL)
  {
    return;
  }

  m_event->create(&prn, ui->checkBox->isChecked());
}

void CSkEventDocDialog::on_checkBox_toggled(bool checked)
{
  QImage image(getSize(), QImage::Format_ARGB32);

  m_event->create(&image, checked);

  QPixmap pixmap = QPixmap::fromImage(image);
  m_label->setPixmap(pixmap);
}

QSize CSkEventDocDialog::getSize()
{
  int width = 800;
  return QSize(width, width * 1.414);
}

void CSkEventDocDialog::on_pushButton_3_clicked()
{
  QFileDialog dlg(this);

  QString file = dlg.getSaveFileName(this, tr("Save PDF File"), "", "PDF (*.pdf)");
  if (!file.isEmpty())
  {
    QPrinter printer(QPrinter::ScreenResolution);

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageMargins(5, 5, 5, 5, QPrinter::Millimeter);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(file);

    qDebug() << printer.paperRect();

    m_event->create(&printer, ui->checkBox->isChecked());
  }
}
