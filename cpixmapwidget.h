#ifndef CPIXMAPWIDGET_H
#define CPIXMAPWIDGET_H

#include <QWidget>

class QPaintEvent;
class QPixmap;

class CPixmapWidget : public QWidget
{
  Q_OBJECT
public:
  explicit CPixmapWidget(QWidget *parent = 0);
  void setPixmap(const QPixmap& pixmap);

protected:
  void paintEvent(QPaintEvent *);

private:
  QPixmap m_pixmap;

signals:

public slots:

};

#endif // CPIXMAPWIDGET_H
