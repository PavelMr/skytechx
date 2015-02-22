#ifndef CDSOGALERYLIST_H
#define CDSOGALERYLIST_H

#include <QDialog>

namespace Ui {
class CDSOGaleryList;
}

class CDSOGaleryList : public QDialog
{
  Q_OBJECT

public:
  explicit CDSOGaleryList(QWidget *parent = 0);
  ~CDSOGaleryList();

protected:
  void changeEvent(QEvent *e);
  void fillList();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_3_clicked();

  void slotDelete();

  void on_pushButton_4_clicked();

private:
  Ui::CDSOGaleryList *ui;
};

#endif // CDSOGALERYLIST_H
