#ifndef SKWATCH_H
#define SKWATCH_H

#include <QWidget>
#include <QPixmap>
#include <QTime>

#define SKW_BLACK           0
#define SKW_WHITE           1
#define SKW_DIGITAL         2

class SkWatch : public QWidget
{
  Q_OBJECT
public:
  explicit SkWatch(QWidget *parent = 0);
  void render();
  void setTime(const QTime time);

public slots:
void setStyle(int style);

protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  int heightForWidth(int w) const;
  QSize sizeHint() const;

private:
  int      m_style;
  QPixmap *m_background;
  QTime    m_time;

signals:

public slots:
};

#endif // SKWATCH_H
