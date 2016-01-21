#ifndef SKCALENDAR_H
#define SKCALENDAR_H

#include <QCalendarWidget>

class SkCalendar : public QCalendarWidget
{
public:
  SkCalendar(QWidget *parent = 0) : QCalendarWidget(parent) {}
  void setSelectedTime(const QTime &time) { m_time = time; updateCells();  }

private:
  QTime m_time;

protected:
  void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
  {
    if (date == QCalendarWidget::selectedDate())
    {
      const int height = 4;

      painter->fillRect(rect, Qt::blue);
      painter->save();
      painter->setPen(Qt::white);
      painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

      int mins = m_time.hour() * 60 + m_time.minute();

      QRect rcBar(rect.left() + 1, rect.bottom() - height, rect.width() - 2, height);
      painter->fillRect(rcBar, Qt::yellow);

      int w = mins * (rcBar.width() / 1440.0);
      QRect rcProgress(rcBar.left(), rcBar.top(), w, rcBar.height());
      painter->fillRect(rcProgress, Qt::red);

      painter->restore();
    }
    else
    {
      QCalendarWidget::paintCell(painter, rect, date);
    }
  }
};

#endif // SKCALENDAR_H

