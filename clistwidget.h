#ifndef CLISTWIDGET_H
#define CLISTWIDGET_H

#include <QListWidget>
#include <QKeyEvent>
#include <QTimer>

class CListWidget : public QListWidget
{
  Q_OBJECT
public:
  explicit CListWidget(QWidget *parent = 0);

protected:
  void keyPressEvent(QKeyEvent *event);

  QTimer m_timer;

private:
  QString m_text;

signals:

public slots:
  void slotTimeout();

};

#endif // CLISTWIDGET_H
