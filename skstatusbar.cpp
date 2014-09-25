#include "skstatusbar.h"

#include <QPushButton>
#include <QPainter>

static QBrush background = QLinearGradient(0, 0, 0, 1);

QPushButton *l1;

SkStatusBar::SkStatusBar(QWidget *parent) :
  QToolBar(parent)
{
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  resize(100, 28);
}

void SkStatusBar::paintEvent(QPaintEvent *event)
{
  QPainter p(this);

  p.fillRect(rect(), background);
}

void SkStatusBar::resizeEvent(QResizeEvent *event)
{
  QLinearGradient g = QLinearGradient(0, 0, 0, height());
  g.setColorAt(0, Qt::white);
  g.setColorAt(1, Qt::gray);
  background = g;

  int x = 0;
  QMapIterator<int, statusBarItem_t> i(m_tItemMap);
  while (i.hasNext())
  {
    i.next();
    statusBarItem_t &item = m_tItemMap[i.key()];

    item.widget->resize(i.value().width, 24);
    item.widget->move(x, 4);

    x += item.widget->width() + 5;
  }
}

void SkStatusBar::addItem(int id, bool textValue, int width, const QString &text, const QString &value)
{
  _ASSERT(!tItemMap.contains(id));

  statusBarItem_t item;
  SkStatusWidget *w = new SkStatusWidget(this);

  item.textValue = textValue;
  item.width = width;
  item.text = text;
  item.value = value;
  item.id = id;
  item.widget = w;
  w->setItem(&item);

  m_tItemMap[id] = item;
}

///////////////////////////////////////

SkStatusWidget::SkStatusWidget(QWidget *parent) :
  QWidget(parent)
{
}

static QSize textMargin = QSize(2, 2);

void SkStatusWidget::paintEvent(QPaintEvent *event)
{
  QPainter p(this);

  //p.fillRect(rect(), Qt::white);

  if (!m_item.textValue)
  {
    p.fillRect(rect(), Qt::white);
    p.drawText(itemRect(), Qt::AlignLeft | Qt::AlignVCenter, m_item.text);
  }
  else
  {
    p.fillRect(rect(), Qt::red);
    QFontMetrics fm(p.font());
    int textWidth = fm.width(m_item.text);

    int m = 1;
    QPolygon poly;
    p.setPen(Qt::black);
    poly.append(QPoint(m + 1, m));
    poly.append(QPoint(m + textWidth + m - 1, m + 1));

    //p.drawLine(m + textWidth + m - 1, m + 1, m + textWidth + m - 1, m + textWidth - m);

    p.setPen(Qt::white);
    poly.append(QPoint(m + textWidth + m - 1, m + height() - 1));
    poly.append(QPoint(m + 1, m + height() - 1));

    p.setBrush(Qt::black);
    p.drawPolygon(poly);


    /*
    QFontMetrics fm(p.font());
    QRect r1 = rect();
    QRect r2 = itemRect();

    r1.setWidth(fm.width(m_item.text));
    p.setPen(Qt::black);
    p.setBrush(Qt::white);
    p.drawRoundRect(r1, 5, 5);
    p.drawText(r1, Qt::AlignCenter, m_item.text);

    r2.setX(r1.right());
    p.drawRoundRect(r2, 5, 5);
    p.drawText(r2, Qt::AlignLeft | Qt::AlignVCenter, m_item.value);
    */
  }
}

QRect SkStatusWidget::itemRect() const
{
  QRect result = rect();

  result.adjust(5, 5, -5, -5);

  return result;
}

void SkStatusWidget::setItem(const statusBarItem_t *item)
{
  m_item = *item;
}
