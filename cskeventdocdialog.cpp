#include "cskeventdocdialog.h"
#include "ui_cskeventdocdialog.h"

#include "skcore.h"

AspectRatioPixmapLabel::AspectRatioPixmapLabel(QWidget *parent) :
    QLabel(parent)
{
  this->setMinimumSize(1,1);
}

void AspectRatioPixmapLabel::setPixmap ( const QPixmap & p)
{
  pix = p;
  QLabel::setPixmap(p);
}

int AspectRatioPixmapLabel::heightForWidth( int width ) const
{
  return ((qreal)pix.height() * width) / pix.width();
}

QSize AspectRatioPixmapLabel::sizeHint() const
{
  int w = this->width();
  return QSize(w, heightForWidth(w));
}

void AspectRatioPixmapLabel::resizeEvent(QResizeEvent *)
{
  QLabel::setPixmap(pix.scaled(QSize(this->size().width() - 10, this->size().height() - 10),
                    Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

CSkEventDocDialog::CSkEventDocDialog(QWidget *parent, const QImage &image) :
  QDialog(parent),
  ui(new Ui::CSkEventDocDialog)
{
  QDesktopWidget dsk;

  ui->setupUi(this);
  resize(dsk.width() * 0.3, dsk.height() * 0.8);

  AspectRatioPixmapLabel *label = new AspectRatioPixmapLabel();
  ui->verticalLayout->addWidget(label);

  QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect();

  QPixmap pixmap = QPixmap::fromImage(image);
  label->setAlignment(Qt::AlignCenter);
  label->setPixmap(pixmap);
  label->setGraphicsEffect(eff);
  label->setMinimumSize(256, 256);

  m_image = image;
}

CSkEventDocDialog::~CSkEventDocDialog()
{
  delete ui;
}

void CSkEventDocDialog::on_pushButton_clicked()
{
  done(DL_OK);
}

#include <QPrinter>

void CSkEventDocDialog::on_pushButton_2_clicked()
{
  QFileDialog dlg(this);

  QString file = dlg.getSaveFileName(this, tr("Save PDF File"), "", "PDF (*.pdf)");
  if (!file.isEmpty())
  {
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);

    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(file);

    //QPainter p(&printer);
    //p.drawImage(QRect(0, 0, printer.width(), printer.height()), m_image);


    QPainter painter;

            painter.begin(&printer);
            painter.setRenderHint(QPainter::Antialiasing);

            double xscale = printer.pageRect().width()/double(width());
            double yscale = printer.pageRect().height()/double(height());
            double scale = qMin(xscale, yscale);
            painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                               printer.paperRect().y() + printer.pageRect().height()/2);
            painter.scale(scale, scale);
            painter.translate(-width()/2, -height()/2);

    render(&painter);
  }
}

