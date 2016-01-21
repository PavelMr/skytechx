#include "skwatch.h"

#include <QPainter>
#include <QResizeEvent>
#include <QLCDNumber>

SkWatch::SkWatch(QWidget *parent) : QWidget(parent)
{
  m_background = NULL;
  m_style = SKW_DIGITAL;
  m_time = QTime(12, 0, 0);

  QSizePolicy qsp(QSizePolicy::Preferred, QSizePolicy::Preferred);

  qsp.setHeightForWidth(true);
  setSizePolicy(qsp);
}

void SkWatch::setStyle(int style)
{
  m_style = style;
  updateGeometry();
  render();
  update();
}

void SkWatch::render()
{
  QRect rc = rect();
  int radius = (rect().width() / 2.0) - 10;
  QPoint center = QPoint(rc.width() / 2, rc.height() / 2);

  if (m_background != NULL)
  {
    delete m_background;
  }

  m_background = new QPixmap(rc.size());
  m_background->fill(Qt::transparent);

  QPainter p(m_background);
  p.setRenderHint(QPainter::Antialiasing);

  if (m_style == SKW_BLACK)
  {
    QRadialGradient gBk(center, radius, center * 0.3);

    gBk.setColorAt(0, QColor(62, 62, 62));
    gBk.setColorAt(1, QColor(0, 0, 0));

    p.setPen(Qt::NoPen);
    p.setBrush(gBk);

    p.drawEllipse(center, radius, radius);

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::darkGray, radius * 0.04));

    p.drawEllipse(center, radius, radius);

    p.setPen(QPen(Qt::white, radius * 0.04));
    p.drawEllipse(center, 5, 5);

    for (int i = 0; i < 60; i++)
    {
      p.save();
      p.translate(center);
      p.rotate(i * 6);

      p.setPen(QPen(Qt::white, 2.5));
      p.drawLine(0, radius * 0.89, 0, radius * 0.97);

      if ((i % 5) == 0)
      {
        p.setPen(QPen(Qt::white, 4.5, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(0, radius * 0.72, 0, radius * 0.86);
      }

      p.restore();
    }
  }
  else
  if (m_style == SKW_WHITE)
  {
    QRadialGradient gBk(center, radius, center * 0.8);

    gBk.setColorAt(0, QColor(222, 222, 222));
    gBk.setColorAt(1, QColor(200, 200, 200));

    p.setPen(Qt::NoPen);
    p.setBrush(gBk);

    p.drawEllipse(center, radius, radius);

    p.setPen(QPen(Qt::black, radius * 0.04));
    p.drawEllipse(center, 5, 5);

    int h = 6;
    for (int i = 0; i < 60; i++)
    {
      p.save();
      p.translate(center);
      p.rotate(i * 6);

      if ((i % 5) == 0)
      {
        p.setPen(Qt::black);
        p.setBrush(Qt::black);
        p.drawEllipse(QPoint(0, radius * 0.94), radius * 0.03, radius * 0.03);

        qreal tx;
        qreal ty;

        p.transform().map(0, radius * 0.8, &tx, &ty);

        p.restore();

        p.save();
        p.setPen(Qt::black);
        p.setFont(QFont("Cooper Std Black", radius * 0.12, QFont::Bold));
        p.drawText(QRect(tx - 100, ty - 100, 200, 200), Qt::AlignCenter, QString::number(h));
        p.restore();

        if (++h > 12) h = 1;

        p.save();
      }
      else
      {
        p.setPen(QPen(Qt::black, 1.5));
        p.drawLine(0, radius * 0.92, 0, radius * 0.97);
      }

      p.restore();
    }
  }
  else
  if (m_style == SKW_DIGITAL)
  {
    QLinearGradient gBk(0, 0, 0, rect().bottom());

    gBk.setColorAt(0, QColor(32, 32, 32));
    gBk.setColorAt(0.2, QColor(64, 64, 64));
    gBk.setColorAt(0.8, QColor(64, 64, 64));
    gBk.setColorAt(1, QColor(32, 32, 32));

    p.setPen(Qt::NoPen);
    p.setBrush(gBk);

    p.drawRect(rect());
  }
}

void SkWatch::setTime(const QTime time)
{
  m_time = time;
  update();
}

void SkWatch::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing);
  p.drawPixmap(0, 0, *m_background);

  QRect rc = rect();
  int radius = (rect().width() / 2.0) - 10;
  QPoint center = QPoint(rc.width() / 2, rc.height() / 2);

  float angHour = 0.5 * (60 * m_time.hour() + m_time.minute());
  float angMin = 6 * m_time.minute();
  float angSec = 6 * m_time.second();

  if (m_style == SKW_BLACK)
  {
    p.save();
    p.translate(center);
    p.rotate(180 + angHour);

    p.setPen(QPen(Qt::white, 6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(0, radius * 0, 0, radius * 0.60);

    p.restore();

    p.save();
    p.translate(center);
    p.rotate(180 + angMin);

    p.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(0, radius * 0, 0, radius * 0.90);

    p.restore();

    p.save();
    p.translate(center);
    p.rotate(180 + angSec);

    p.setPen(QPen(Qt::white, 1.5, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(0, -radius * 0.2, 0, radius * 0.92);

    p.restore();

    QRadialGradient gr(center, radius, center);

    int r = 0;
    int g = 0;
    int b = 0;

    gr.setColorAt(0, QColor(r, g, b, 5));
    gr.setColorAt(0.92, QColor(r, g, b, 10));
    gr.setColorAt(0.95, QColor(r, g, b, 220));
    gr.setColorAt(1, QColor(r, g, b, 255));

    p.setPen(Qt::NoPen);
    p.setBrush(gr);

    p.drawEllipse(center, radius, radius);
  }
  else
  if (m_style == SKW_WHITE)
  {
    p.save();
    p.translate(center);
    p.rotate(180 + angHour);

    p.setPen(QPen(Qt::black, 6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(0, radius * 0, 0, radius * 0.60);

    p.restore();

    p.save();
    p.translate(center);
    p.rotate(180 + angSec);

    p.setPen(QPen(Qt::red, 1.5, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(0, -radius * 0.2, 0, radius * 0.92);

    p.restore();

    p.save();
    p.translate(center);
    p.rotate(180 + angMin);

    p.setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(0, radius * 0, 0, radius * 0.90);

    p.restore();

    QRadialGradient gr(center, radius, center);

    int r = 0;
    int g = 0;
    int b = 10;

    gr.setColorAt(0, QColor(r, g, b, 0));
    gr.setColorAt(0.90, QColor(r, g, b, 16));
    gr.setColorAt(0.95, QColor(r, g, b, 32));
    gr.setColorAt(1, QColor(r, g, b, 225));

    p.setPen(Qt::NoPen);
    p.setBrush(gr);

    p.drawEllipse(center, radius, radius);
  }
  else
  if (m_style == SKW_DIGITAL)
  {
    QLCDNumber lcd(8);

    QPalette pal;
    pal.setColor(QPalette::WindowText, Qt::lightGray);

    lcd.resize(width(), 48);
    lcd.setSegmentStyle(QLCDNumber::Flat);

    lcd.setPalette(pal);
    lcd.setFrameStyle(QFrame::NoFrame);
    lcd.setAttribute(Qt::WA_NoSystemBackground);
    lcd.setAttribute(Qt::WA_TranslucentBackground);
    lcd.display(QString("%1:%2:%3").arg(m_time.hour(), 2, 10, QChar('0')).
                                    arg(m_time.minute(), 2, 10, QChar('0')).
                                    arg(m_time.second(), 2, 10, QChar('0')));

    QPixmap pix = lcd.grab();

    p.drawPixmap(0, height() / 2 - pix.height() / 2, pix);
  }
}

void SkWatch::resizeEvent(QResizeEvent *)
{
  render();
}

int SkWatch::heightForWidth(int w) const
{
  if (m_style == SKW_DIGITAL)
    return w / 2;

  return w;
}

QSize SkWatch::sizeHint() const
{
  return QSize(256, 256);
}

