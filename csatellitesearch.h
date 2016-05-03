#ifndef CSATELLITESEARCH_H
#define CSATELLITESEARCH_H

#include <QDialog>

#include "cmapview.h"
#include "mapobj.h"

namespace Ui {
class CSatelliteSearch;
}

class CSatelliteSearch : public QDialog
{
  Q_OBJECT

public:
  explicit CSatelliteSearch(mapView_t *view, QWidget *parent = 0);
  ~CSatelliteSearch();
  double m_ra;
  double m_dec;
  double m_fov;
  mapObj_t m_mapObj;


private slots:
  void on_pushButton_2_clicked();

  void on_listView_doubleClicked(const QModelIndex &index);

  void on_pushButton_clicked();

  void slotSelChange(QModelIndex &);

private:
  Ui::CSatelliteSearch *ui;
  mapView_t *m_mapView;
};

#endif // CSATELLITESEARCH_H
