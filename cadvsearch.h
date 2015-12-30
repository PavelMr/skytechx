#ifndef CADVSEARCH_H
#define CADVSEARCH_H

#include "cmapview.h"
#include "skcore.h"
#include "mapobj.h"

#include <QDialog>

namespace Ui {
class CAdvSearch;
}

class CAdvSearch : public QDialog
{
  Q_OBJECT

public:
  explicit CAdvSearch(QWidget *parent, mapView_t *view);
  ~CAdvSearch();

  mapObj_t m_mapObj;
  double m_ra;
  double m_dec;
  double m_fov;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_lineEdit_textChanged(const QString &arg1);

private:
  Ui::CAdvSearch *ui;

  mapView_t m_mapView;
};

#endif // CADVSEARCH_H
