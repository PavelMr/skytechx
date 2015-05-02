#ifndef CBINOCULAR_H
#define CBINOCULAR_H

#include <QDialog>
#include <QListView>

typedef struct
{
  QString name;
  double  diameter; // in mm
  double  fov;   // in arc sec.
  double  magnification;
} binocular_t;

namespace Ui {
class CBinocular;
}

class CBinocular : public QDialog
{
  Q_OBJECT

public:
  explicit CBinocular(QWidget *parent = 0);
  ~CBinocular();
  double m_fov;
  QString m_text;

private slots:
  void slotIndexChanged(QModelIndex current, QModelIndex prev);

  void on_pushButton_3_clicked();

  void deleteItem();

  void on_pushButton_7_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_listWidget_doubleClicked(const QModelIndex &index);

  void on_pushButton_4_clicked();

private:
  Ui::CBinocular *ui;

  void load();
  void save();
};

#endif // CBINOCULAR_H
