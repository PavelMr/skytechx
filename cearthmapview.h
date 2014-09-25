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

    QList <QPointF> m_list;

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
