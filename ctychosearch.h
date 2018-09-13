#ifndef CTYCHOSEARCH_H
#define CTYCHOSEARCH_H

#include <QDialog>

#include "skcore.h"
#include "mapobj.h"

namespace Ui {
class CTychoSearch;
}

class CTychoSearch : public QDialog
{
  Q_OBJECT
  
public:
  explicit CTychoSearch(QWidget *parent, double epoch);
  ~CTychoSearch();
  radec_t m_rd;
  mapObj_t m_mapObj;
  
protected:
  void changeEvent(QEvent *e);
  void notFound(QWidget *w);
  void getStar(void);  

  int      m_reg;
  int      m_index;
  double   m_yr;

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

private:
  Ui::CTychoSearch *ui;
};

#endif // CTYCHOSEARCH_H
