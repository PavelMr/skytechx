/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CEARTHMAPVIEW_H
#define CEARTHMAPVIEW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>

class CEarthMapView : public QWidget
{
  Q_OBJECT
public:
  explicit CEarthMapView(QWidget *parent = 0);
  void setPixmap(QPixmap *pixmap);
  void setOldLonLat(double lon, double lat);
  void setList(QList <QPointF> &list) { m_list = list; }
  void resetView();

protected:
    QPixmap *m_pixmap;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void wheelEvent(QWheelEvent *e);

    QList <QPointF> m_list;         // cities list
    QList <QList <QPointF>> m_data; // boundaries data

    int m_lastX;
    int m_lastY;

    double m_cx;
    double m_cy;
    double m_scale;
    double m_maxScale;

    double m_oldLon;
    double m_oldLat;

    bool   m_move;
    bool   m_set;
    double m_lon;
    double m_lat;

    void coord2Screen(double lon, double lat, int &x, int &y);
    void screen2Coord(int x, int y, double &lon, double &lat);


signals:

    void sigLonLatChange(double lon, double lat);
    void sigSiteChange(double lon, double lat);

public slots:

};

#endif // CEARTHMAPVIEW_H
