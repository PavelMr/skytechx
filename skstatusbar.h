#ifndef SKSTATUSBAR_H
#define SKSTATUSBAR_H

#include <QToolBar>
#include <QMap>

typedef struct
{
  QWidget *widget;
  bool textValue;
  QString text;
  QString value;
  int width;
  int id;

} statusBarItem_t;

class SkStatusWidget : public QWidget
{
public:
  explicit SkStatusWidget(QWidget *parent = 0);
  void paintEvent(QPaintEvent *event);
  void setItem(const statusBarItem_t *item);
  QRect itemRect() const;

private:
  statusBarItem_t m_item;
};

class SkStatusBar : public QToolBar
{
  Q_OBJECT

public:
  explicit SkStatusBar(QWidget *parent = 0);
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);

  void addItem(int id, bool textValue = false, int width = 0, const QString& text = "", const QString& value = "");

signals:

public slots:

private:
  QMap <int, statusBarItem_t> m_tItemMap;

};

#endif // SKSTATUSBAR_H
