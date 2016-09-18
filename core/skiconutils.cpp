#include "skiconutils.h"

#include <QIcon>

SkIconUtils::SkIconUtils()
{

}

QIcon SkIconUtils::createFromText(int sx, int sy, const QString &text)
{
  QPixmap pixmap(sx, sy);

  pixmap.fill(Qt::transparent);

  QPainter p;
  QFont font;

  p.begin(&pixmap);  
  font.setBold(true);
  font.setFamily("verdana");
  font.setPointSize(sy * 0.32);
  p.setFont(font);
  p.setPen(Qt::black);
  p.drawText(QRect(0, 0, sx, sy), Qt::AlignCenter, text);
  p.end();

  return QIcon(pixmap);
}


