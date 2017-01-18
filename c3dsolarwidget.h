/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#ifndef C3DSOLARWIDGET_H
#define C3DSOLARWIDGET_H

#include "skcore.h"
#include "cmapview.h"

#include <QFrame>
#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QMouseEvent>

class C3DSolarWidget : public QWidget
{
  Q_OBJECT
public:
  explicit C3DSolarWidget(QWidget *parent = 0);
  void setView(mapView_t *view, bool genOrbit = false);
  void generateComet(int index, double time, double period, bool isComet);
  void setShowHeight(bool show);
  void setShowEclipticPlane(bool show);
  void setShowRadius(bool show);
  void setLockAt(int index);
  void setViewParam(double yaw = CM_UNDEF, double pitch = CM_UNDEF, double x = CM_UNDEF, double y = CM_UNDEF, double z = CM_UNDEF, bool updateView = true);
  void removeOrbit();
  QPixmap *getPixmap();
  void setBkColor(const QColor &color);

protected:
  void paintEvent(QPaintEvent *);
  void wheelEvent(QWheelEvent *);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void setup();

  //SKMATRIX m_projMatrix;
  //double m_width, m_height;

  void generateOrbits();

  QColor    m_bkColor;
  bool      m_isComet;
  int       m_lockAt;
  bool      m_showEclipticPlane;
  bool      m_showHeight;
  bool      m_showRadius;
  mapView_t m_view;
  double    m_scale;
  QVector3D m_translate;
  bool      m_drag;
  bool      m_rotate;
  QPoint    m_lastPoint;
  double    m_yaw;
  double    m_pitch;
  int       m_index;
  QPixmap  *m_pixmap;

public slots:

};

#endif // C3DSOLARWIDGET_H
