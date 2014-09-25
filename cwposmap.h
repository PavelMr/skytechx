#ifndef CWPOSMAP_H
#define CWPOSMAP_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
class CWPosMap;
}

class CWPosMap : public QDialog
{
  Q_OBJECT

public:
  explicit CWPosMap(QWidget *parent, double lon, double lat, QList<QPointF> &list);
  ~CWPosMap();

  double   m_lon;
  double   m_lat;

protected:
  void changeEvent(QEvent *e);

  QPixmap *m_pix;
  double   m_oldLon;
  double   m_oldLat;
  QList <QPointF> m_list;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void slotLonLatChanged(double lon, double lat);
  void slotSiteChanged(double lon, double lat);

  void on_pushButton_3_clicked();

private:
  Ui::CWPosMap *ui;
};

#endif // CWPOSMAP_H
