#ifndef CDRAWINGLIST_H
#define CDRAWINGLIST_H

#include <QDialog>
#include <QStandardItem>

namespace Ui {
class CDrawingList;
}

class CDrawingList : public QDialog
{
  Q_OBJECT

public:
  explicit CDrawingList(QWidget *parent = 0);
  ~CDrawingList();
  double m_ra;
  double m_dec;
  double m_fov;

protected:
  void changeEvent(QEvent *e);

  QStandardItemModel *m_model;

private slots:
  void on_pushButton_2_clicked();
  void slotDelete();
  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_clicked();

private:
  Ui::CDrawingList *ui;
};

#endif // CDRAWINGLIST_H
