#ifndef CSOLARSYSTEM_H
#define CSOLARSYSTEM_H

#include "cmapview.h"

#include <QDialog>
#include <QPainter>
#include <QResizeEvent>

namespace Ui {
class CSolarSystem;
}

class CSolarSystem : public QDialog
{
  Q_OBJECT

public:
  explicit CSolarSystem(QWidget *parent, mapView_t *view);
  ~CSolarSystem();

protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *e);

  void generatePixmap();

  QPixmap   *m_pixmap;
  mapView_t  m_view;
  QTimer    *m_timer;
  bool       m_inner;

private slots:
  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

private:
  Ui::CSolarSystem *ui;
};

#endif // CSOLARSYSTEM_H
