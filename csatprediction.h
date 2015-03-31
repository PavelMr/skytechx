#ifndef CSATPREDICTION_H
#define CSATPREDICTION_H

#include "cmapview.h"
#include "csgp4.h"

#include <QDialog>
#include <QThread>

class CSatPredictionSolver : public QThread
{
  Q_OBJECT

public:
  CSatPredictionSolver(int index, double fromJD, double toJD, mapView_t *view);
  void run(void);

private:
  double findCrossingPoint(double initial_time1, double initial_time2, bool finding_aos);

  mapView_t m_view;
  int m_index;
  double m_fromJD;
  double m_toJD;
};

namespace Ui {
class CSatPrediction;
}

class CSatPrediction : public QDialog
{
  Q_OBJECT

public:
  explicit CSatPrediction(QWidget *parent, mapView_t *view);
  ~CSatPrediction();

protected:
  void fillList();

private slots:
  void slotSelChange(QModelIndex &index);

  void on_pushButton_clicked();

private:
  Ui::CSatPrediction *ui;
  mapView_t m_view;
  QList <QThread *> m_thread;
};

#endif // CSATPREDICTION_H
