#ifndef CINSERTFINDER_H
#define CINSERTFINDER_H

#include "csimplelist.h"

#include <QDialog>

typedef struct
{
  QString name;
  double  fov;
  double  magnification;
} finderScope_t;


namespace Ui {
class CInsertFinder;
}

class CInsertFinder : public QDialog
{
  Q_OBJECT

public:
  explicit CInsertFinder(QWidget *parent = 0);
  ~CInsertFinder();

  double m_fov;
  QString m_text;

private:
  Ui::CInsertFinder *ui;

  void load();
  void save();

private slots:
  void slotIndexChanged(QModelIndex current, QModelIndex prev);
  void deleteItem();
  void on_pushButton_2_clicked();
  void on_pushButton_clicked();
  void on_listWidget_doubleClicked(const QModelIndex &index);
  void on_pushButton_4_clicked();
  void on_pushButton_3_clicked();
  void on_pushButton_5_clicked();
};

#endif // CINSERTFINDER_H
