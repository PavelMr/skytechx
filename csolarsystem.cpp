#include "csolarsystem.h"
#include "ui_csolarsystem.h"
#include "castro.h"

static QList <QPointF> ptsList[8];

CSolarSystem::CSolarSystem(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CSolarSystem)
{
  ui->setupUi(this);

  m_inner = true;
  m_view = *view;
  m_pixmap = NULL;

  ui->comboBox->addItem(tr("Inner Solar system"));
  ui->comboBox->addItem(tr("Outer Solar system"));

  for (int i = 0; i < 8; i++)
  {
    ptsList[i].clear();
  }

  double jd = view->jd;
  CAstro ast;
  double steps[8] = {8, 3, 6, 10, 15, 20, 30, 40};

  for (int i = PT_SUN; i <= PT_NEPTUNE; i++)
  {
    orbit_t o;
    m_view.jd = jd;

    ast.setParam(&m_view);
    ast.calcPlanet(i, &o);

    double last = o.hLon;
    double total = 0;

    if (i == 0)
      ptsList[i].append(QPointF(o.eRect[0], o.eRect[1]));
    else
      ptsList[i].append(QPointF(o.hRect[0], o.hRect[1]));

    do
    {
      m_view.jd += steps[i];
      ast.setParam(&m_view);
      ast.calcPlanet(i, &o);

      double delta = o.hLon - last;

      if (delta < 0)
      {
        delta = fabs(delta + R360);
      }

      total += delta;
      last = o.hLon;

      if (total > R360)
      {
        break;
      }

      if (i == 0)
        ptsList[i].append(QPointF(o.eRect[0], o.eRect[1]));
      else
        ptsList[i].append(QPointF(o.hRect[0], o.hRect[1]));

    } while (1);
  }

  m_view.jd = jd;
  generatePixmap();
}

CSolarSystem::~CSolarSystem()
{
  delete m_pixmap;
  delete ui;
}

void CSolarSystem::changeEvent(QEvent *e)
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

void CSolarSystem::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.drawPixmap(ui->frame->pos(), *m_pixmap);
}

void CSolarSystem::resizeEvent(QResizeEvent *)
{
  generatePixmap();
}

void CSolarSystem::generatePixmap()
{
  if (m_pixmap)
  {
    delete m_pixmap;
  }

  QColor color[8] = {Qt::blue, Qt::lightGray, Qt::yellow, Qt::red, Qt::darkYellow, Qt::yellow, Qt::green, Qt::blue};

  m_pixmap = new QPixmap(ui->frame->size());
  m_pixmap->fill(Qt::black);

  QPainter p(m_pixmap);

  QPoint opt[8];

  p.setRenderHint(QPainter::Antialiasing, true);

  p.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
  p.drawLine(0, m_pixmap->height() / 2, m_pixmap->width(), m_pixmap->height() / 2);
  p.drawLine(m_pixmap->width() / 2, 0, m_pixmap->width() / 2, m_pixmap->height());

  p.setPen(Qt::white);
  p.setFont(QFont("arial", 12));
  p.drawText(5, 20, "JD " + QString::number(m_view.jd, 'f', 6));
  p.drawText(5, 40, getStrDate(m_view.jd, m_view.geo.tz) + " " + getStrTime(m_view.jd, m_view.geo.tz));

  p.save();
  p.translate(m_pixmap->width() / 2, m_pixmap->height() / 2);
  p.scale(1, -1);

  p.setPen(Qt::yellow);
  p.setBrush(Qt::yellow);
  p.drawEllipse(QPoint(0, 0), 10, 10);

  p.setPen(Qt::gray);

  int from, to;
  double factor;

  if (m_inner)
  {
    from = PT_SUN;
    to = PT_MARS;
    factor = 4.2;
  }
  else
  {
    from = PT_JUPITER;
    to = PT_NEPTUNE;
    factor = 80.0;
  }

  CAstro ast;

  for (int i = from; i <= to; i++)
  {
    double scale = qMin(width(), height()) / factor;

    orbit_t o;

    ast.setParam(&m_view);
    ast.calcPlanet(i, &o);

    for (int j = 0; j < ptsList[i].count(); j++)
    {
      QPointF pt1 = ptsList[i][j];
      QPointF pt2 = ptsList[i][(j + 1) % ptsList[i].count()];

      p.setPen(Qt::gray);
      p.drawLine(pt1 * scale, pt2 * scale);
    }

    p.setPen(color[i]);
    p.setBrush(color[i]);
    QPoint pos;
    if (i == 0)
      pos = QPoint(o.eRect[0] * scale, o.eRect[1] * scale);
    else
      pos = QPoint(o.hRect[0] * scale, o.hRect[1] * scale);

    p.drawEllipse(pos, 5, 5);

    QTransform tr = p.transform();
    opt[i] = tr.map(pos);
  }

  p.restore();
  p.setPen(Qt::white);

  for (int i = from; i <= to; i++)
    p.drawText(QPoint(opt[i].x() + 5, opt[i].y() - 5), (i == 0) ?  tr("Earth") : ast.getName(i));
}



void CSolarSystem::on_comboBox_currentIndexChanged(int index)
{
  if (index == 0)
  {
    m_inner = true;
  }
  else
  {
    m_inner = false;
  }

  generatePixmap();
  update();
}

void CSolarSystem::on_pushButton_clicked()
{
  done(DL_OK);
}

void CSolarSystem::on_pushButton_2_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd, &dt);
  dt = dt.addYears(-1);
  m_view.jd = jdGetJDFrom_DateTime(&dt);

  generatePixmap();
  update();
}

void CSolarSystem::on_pushButton_3_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd, &dt);
  dt = dt.addYears(1);
  m_view.jd = jdGetJDFrom_DateTime(&dt);

  generatePixmap();
  update();
}

void CSolarSystem::on_pushButton_4_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd, &dt);
  dt = dt.addMonths(-1);
  m_view.jd = jdGetJDFrom_DateTime(&dt);

  generatePixmap();
  update();
}

void CSolarSystem::on_pushButton_5_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd, &dt);
  dt = dt.addMonths(1);
  m_view.jd = jdGetJDFrom_DateTime(&dt);

  generatePixmap();
  update();
}

void CSolarSystem::on_pushButton_6_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd, &dt);
  dt = dt.addDays(-1);
  m_view.jd = jdGetJDFrom_DateTime(&dt);

  generatePixmap();
  update();
}

void CSolarSystem::on_pushButton_7_clicked()
{
  QDateTime dt;

  jdConvertJDTo_DateTime(m_view.jd, &dt);
  dt = dt.addDays(1);
  m_view.jd = jdGetJDFrom_DateTime(&dt);

  generatePixmap();
  update();
}
