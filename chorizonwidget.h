#ifndef CHORIZONWIDGET_H
#define CHORIZONWIDGET_H

#include "background.h"

#include <QFrame>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QList>
#include <QPointF>
#include <QMap>
#include <QKeyEvent>
#include <QPlainTextEdit>

class CHorizonWidget : public QFrame
{
  Q_OBJECT
public:
  explicit CHorizonWidget(QWidget *parent = 0);
  void setEditor(QPlainTextEdit *editor) { m_editor = editor; updateData(); }
  void setCurrentText(const QString &text) { m_currentText = text; }
  bool load(const QString &name);
  bool save(const QString &name);
  void removeTexture();
  void setTexture(const QString &name);

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  int heightForWidth(int) const;
  void updateData();

  QSize sizeHint() const {
          return QSize(720, heightForWidth(300));
      }

signals:

private:

  CBackground background;

  typedef struct
  {
    int azm;
    QString label;
  } label_t;

  QPlainTextEdit *m_editor;
  int m_azm;
  double m_alt;
  QList <QPointF> m_list;
  double m_altHorizon[360];
  QMap <int, double> m_map;
  QPoint m_lastPoint;
  int m_index;
  bool m_drag;
  int m_labelDrag;
  int m_left;
  int m_right;
  QList <label_t> m_label;
  QString m_currentText;
  QString m_textureName;

  void getPos(const QPoint &pos, int &azm, double &alt);
  void getPos(double azm, double alt, double &x, double &y);


public slots:

};

#endif // CHORIZONWIDGET_H
