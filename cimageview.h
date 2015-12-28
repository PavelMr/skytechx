#ifndef CIMAGEVIEW_H
#define CIMAGEVIEW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "skcore.h"

#define IV_NO_IMAGE         0
#define IV_WAITING          1
#define IV_IMAGE            2

class CImageView : public QWidget
{
  Q_OBJECT
public:
  explicit CImageView(QWidget *parent = 0);
          ~CImageView();
  void setSource(QPixmap *pix);
  void setSourceNoImage();
  void setSourceWaiting();
  QPixmap *getSource(void);
  void setOriginalSize();

protected:
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent *);

  QPushButton *m_btReset;
  QPushButton *m_bt100;
  QPixmap *m_pix;
  double   m_cx;
  double   m_cy;
  double   m_scale;
  int      m_lastX;
  int      m_lastY;
  int      m_type;
  QMovie  *m_waiting;
  QLabel  *m_label;
  bool     m_firstTime;

signals:

public slots:
  void slotReset(void);
  void slot100(void);
};

#endif // CIMAGEVIEW_H
