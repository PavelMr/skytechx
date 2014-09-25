#ifndef CEPHLIST_H
#define CEPHLIST_H

#include <QDialog>
#include "cmapview.h"

namespace Ui {
  class CEphList;
}

class CEphList : public QDialog
{
  Q_OBJECT

public:
  explicit CEphList(QWidget *parent, mapView_t *view);
  ~CEphList();

protected:
  mapView_t m_view;
  bool showNoObjectSelected(int obj);

private slots:
  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_checkBox_2_toggled(bool checked);

  void on_pushButton_2_clicked();

  void on_tabWidget_currentChanged(int index);

  void on_listWidget_currentRowChanged(int currentRow);

private:
  Ui::CEphList *ui;
};

#endif // CEPHLIST_H
