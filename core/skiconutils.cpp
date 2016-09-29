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
  font.setFamily("arial");
  font.setPixelSize(sy * 0.48);
  font.setWeight(99);
  p.setFont(font);  
  p.setPen(QColor(32, 32, 32));
  p.drawText(QRect(0, 0, sx, sy), Qt::AlignCenter, text);
  p.end();

  return QIcon(pixmap);
}


