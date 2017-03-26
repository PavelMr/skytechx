#include "c3dsolar.h"
#include "ui_c3dsolar.h"
#include "ccomdlg.h"
#include "casterdlg.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>

bool g_comAstChanged = false;

static int g_bkColorIndex = 0;
static int g_quiet = true;
static int g_cb = 0;
static int g_cb2 = 0;
static int g_cb3 = 0;
static bool g_cbx = false;
static bool g_cbx2 = true;
static bool g_cbx3 = false;
static int g_sb = 500;
static int g_sb2 = 1;

C3DSolar::C3DSolar(mapView_t *view, QWidget *parent, bool isComet, int index) :
  QDialog(parent),
  ui(new Ui::C3DSolar)
{
  ui->setupUi(this);

  ui->frame->setView(view, true);
  m_view = *view;
  m_jd = m_view.jd;

  ui->comboBox_4->addItem(tr("Black"));
  ui->comboBox_4->addItem(tr("Dark red"));
  ui->comboBox_4->addItem(tr("Blue"));
  ui->comboBox_4->setCurrentIndex(g_bkColorIndex);

  setWindowFlags(Qt::Window);

  QTimer *timer = new QTimer(this);

  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
  timer->start(25);

  ui->comboBox_2->addItem(tr("None"), -1);
  ui->comboBox_2->addItem(tr("Comet/Asteroid"), -2);
  ui->comboBox_2->addItem(tr("Sun"), PT_SUN);
  ui->comboBox_2->addItem(tr("Mercury"), PT_MERCURY);
  ui->comboBox_2->addItem(tr("Venus"), PT_VENUS);
  ui->comboBox_2->addItem(tr("Earth"), 20);
  ui->comboBox_2->addItem(tr("Mars"), PT_MARS);
  ui->comboBox_2->addItem(tr("Jupiter"), PT_JUPITER);
  ui->comboBox_2->addItem(tr("Saturn"), PT_SATURN);
  ui->comboBox_2->addItem(tr("Uranus"), PT_URANUS);
  ui->comboBox_2->addItem(tr("Neptune"), PT_NEPTUNE);

  ui->frame->setShowHeight(ui->checkBox->isChecked());
  ui->frame->setShowEclipticPlane(ui->checkBox_2->isChecked());
  ui->frame->setShowRadius(ui->checkBox_3->isChecked());

  ui->comboBox->addItem(tr("None"), -1);
  for (int i = 0; i < tComets.count(); i++)
  {
    if (tComets[i].selected)
    {
      ui->comboBox->addItem(tComets[i].name, i);
    }
  }

  ui->comboBox_3->addItem(tr("None"), -1);
  for (int i = 0; i < tAsteroids.count(); i++)
  {
    if (tAsteroids[i].selected)
    {
      ui->comboBox_3->addItem(tAsteroids[i].name, i);
    }
  }

  updateData();

  if (isComet && index >= 0)
  {
    int idx = ui->comboBox->findData(index);
    if (idx >= 0)
    {
      ui->comboBox->setCurrentIndex(idx);
      ui->frame->generateComet(index, ui->spinBox->value(), ui->spinBox_2->value(), true);
    }
  }
  if (!isComet && index >= 0)
  {
    int idx = ui->comboBox_3->findData(index);
    if (idx >= 0)
    {
      ui->comboBox_3->setCurrentIndex(idx);
      ui->frame->generateComet(index, ui->spinBox->value(), ui->spinBox_2->value(), false);
    }
  }

  g_quiet = true;

  if (index == -1)
  {
    if (!g_comAstChanged)
    {
      ui->comboBox->setCurrentIndex(g_cb);
      ui->comboBox_3->setCurrentIndex(g_cb3);
    }
    else
    {
      ui->comboBox->setCurrentIndex(0);
      ui->comboBox_3->setCurrentIndex(0);
    }
  }

  g_comAstChanged = false;

  ui->comboBox_2->setCurrentIndex(g_cb2);
  ui->checkBox->setChecked(g_cbx);
  ui->checkBox_2->setChecked(g_cbx2);
  ui->checkBox_3->setChecked(g_cbx3);
  ui->spinBox->setValue(g_sb);
  ui->spinBox_2->setValue(g_sb2);

  on_pushButton_8_clicked();

  g_quiet = false;
}

void C3DSolar::saveAll()
{
  g_cb = ui->comboBox->currentIndex();
  g_cb2 = ui->comboBox_2->currentIndex();
  g_cb3 = ui->comboBox_3->currentIndex();
  g_cbx = ui->checkBox->isChecked();
  g_cbx2 = ui->checkBox_2->isChecked();
  g_cbx3 = ui->checkBox_2->isChecked();
  g_sb = ui->spinBox->value();
  g_sb2 = ui->spinBox_2->value();
  g_bkColorIndex = ui->comboBox_4->currentIndex();
}

C3DSolar::~C3DSolar()
{
  saveAll();
  delete ui;
}

void C3DSolar::on_pushButton_2_clicked()
{
  m_jd--;
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_3_clicked()
{
  m_jd++;
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_checkBox_toggled(bool checked)
{
  ui->frame->setShowHeight(checked);
}

void C3DSolar::on_pushButton_4_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addMonths(-1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_5_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addMonths(+1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_6_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addYears(-1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_7_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);
  dt = dt.addYears(+1);
  m_jd = jdGetJDFrom_DateTime(&dt);

  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::showError()
{
  if (!g_quiet)
  {
    msgBoxError(this, tr("Nothing to show!"));
  }
}

void C3DSolar::on_pushButton_8_clicked()
{
  bool isComet = true;

  if (ui->comboBox->currentIndex() == -1)
  {
    showError();
    return;
  }
  int index = ui->comboBox->currentData().toInt();
  if (index == -1)
  {
    if (ui->comboBox_3->currentIndex() == -1)
    {
      showError();
      return;
    }
    index = ui->comboBox_3->currentData().toInt();
    if (index == -1)
    {
      showError();
      return;
    }
    isComet = false;
  }
  ui->frame->generateComet(index, ui->spinBox->value(), ui->spinBox_2->value(), isComet);
}

void C3DSolar::updateData()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_jd, &dt);

  ui->dateEdit->blockSignals(true);
  ui->dateEdit->setDateTime(dt);
  ui->dateEdit->blockSignals(false);
  ui->label_4->setText(getStrDateTime(m_jd, m_view.geo.tz));
}

void C3DSolar::on_pushButton_9_clicked()
{
  ui->frame->setViewParam(0, R180);
}

void C3DSolar::on_pushButton_10_clicked()
{
  ui->frame->setViewParam(CM_UNDEF, CM_UNDEF, 0, 0, 0);
}

void C3DSolar::on_dateEdit_dateChanged(const QDate &date)
{
  QDateTime dt(date, QTime(12, 0, 0));

  m_jd = jdGetJDFrom_DateTime(&dt);
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}

void C3DSolar::on_pushButton_11_clicked()
{
  m_jd = jdGetCurrentJD();
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}


void C3DSolar::on_horizontalSlider_sliderReleased()
{
  ui->horizontalSlider->setValue(0);
}

void C3DSolar::slotTimer()
{
  QEasingCurve curve(QEasingCurve::InCubic);
  if (ui->horizontalSlider->value() == 0)
  {
    return;
  }
  m_jd += (ui->horizontalSlider->value() > 0 ? +1 : -1) * curve.valueForProgress(qAbs(ui->horizontalSlider->value()) / 100.0) * 50.0;
  m_view.jd = m_jd;
  ui->frame->setView(&m_view);
  updateData();
}


void C3DSolar::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void C3DSolar::on_pushButton_12_clicked()
{
  bool isComet = true;
  if (ui->comboBox->currentIndex() == -1)
  {
    return;
  }
  int index = ui->comboBox->currentData().toInt();
  if (index == -1)
  {
    isComet = false;

    if (ui->comboBox_3->currentIndex() == -1)
    {
      return;
    }
    index = ui->comboBox_3->currentData().toInt();
    if (index == -1)
    {
      return;
    }
  }

  if (isComet)
  {
    comet_t *a = &tComets[index];

    if (a->e < 0.99)
    {
      double aa = a->q / (1.0 - a->e);
      double P = 1.00004024 * pow(aa, 1.5);
      ui->spinBox->setValue(P * 365.25 * 0.5 * 1.02);
    }
    else
    {
      msgBoxError(this, tr("Comet is not periodical!"));
    }
  }
  else
  {
    asteroid_t *a = &tAsteroids[index];

    double P = 365.256898326 * pow(a->a, 1.5);
    ui->spinBox->setValue(P * 0.5 * 1.02);
  }
}

void C3DSolar::on_pushButton_13_clicked()
{
  ui->frame->removeOrbit();
}

void C3DSolar::on_checkBox_2_toggled(bool checked)
{
  ui->frame->setShowEclipticPlane(checked);
}

void C3DSolar::on_comboBox_2_currentIndexChanged(int /*index*/)
{
  int data = ui->comboBox_2->currentData().toInt();
  ui->frame->setLockAt(data);
}

void C3DSolar::on_comboBox_currentIndexChanged(int)
{
  if (ui->comboBox_3->signalsBlocked())
  {
    return;
  }
  ui->comboBox->blockSignals(true);
  ui->comboBox_3->setCurrentIndex(0);
  ui->comboBox->blockSignals(false);
}

void C3DSolar::on_comboBox_3_currentIndexChanged(int)
{
  if (ui->comboBox->signalsBlocked())
  {
    return;
  }
  ui->comboBox_3->blockSignals(true);
  ui->comboBox->setCurrentIndex(0);
  ui->comboBox_3->blockSignals(false);
}

void C3DSolar::on_pushButton_14_clicked()
{  
  done(DL_OK);
}
double C3DSolar::jd() const
{
  return m_view.jd;
}


void C3DSolar::on_checkBox_3_toggled(bool checked)
{
  ui->frame->setShowRadius(checked);
}

void C3DSolar::on_pb_clipboard_clicked()
{
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setPixmap(ui->frame->grab());
}

void C3DSolar::on_pb_print_clicked()
{
  QPrinter prn(QPrinter::ScreenResolution);
  QPrintDialog dlg(&prn, this);

  prn.setOrientation(QPrinter::Landscape);
  prn.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

  if (dlg.exec() == DL_CANCEL)
  {
    return;
  }

  printWidget(&prn);
}

void C3DSolar::on_pb_preview_clicked()
{
  QPrintPreviewDialog dlg(this);

  dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowMaximizeButtonHint);
  dlg.printer()->setOrientation(QPrinter::Landscape);
  dlg.printer()->setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
  dlg.resize(1000, 1000 / 1.333);
  dlg.ensurePolished();

  connect(&dlg, SIGNAL(paintRequested(QPrinter*)), SLOT(slotPrintPreview(QPrinter*)));
  dlg.exec();
}

void C3DSolar::slotPrintPreview(QPrinter *printer)
{
  printWidget(printer);
}


void C3DSolar::printWidget(QPrinter *printer)
{
  QPixmap *pixmap = ui->frame->getPixmap();
  QImage image = pixmap->toImage();

  image = image.convertToFormat(QImage::Format_Grayscale8);

  QPainter p(printer);

  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  //image
  image.invertPixels();
  image = image.scaled(p.device()->width(), p.device()->height(),
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation);

  int px = p.device()->width() / 2 - image.width() / 2;
  int py = p.device()->height() / 2 - image.height() / 2;

  p.drawImage(px, py, image);
  p.setPen(Qt::black);
  p.setBrush(Qt::NoBrush);
  p.drawRect(QRect(QPoint(px, py), image.size()));
  p.end();
}

void C3DSolar::on_comboBox_4_currentIndexChanged(int index)
{
  switch (index)
  {
    case 0:
      ui->frame->setBkColor(Qt::black);
      break;
    case 1:
      ui->frame->setBkColor(QColor(32, 0, 0));
      break;
    case 2:
      ui->frame->setBkColor(QColor(0, 55, 94));
      break;
  }
}
