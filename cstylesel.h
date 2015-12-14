#ifndef CSTYLESEL_H
#define CSTYLESEL_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

namespace Ui {
  class CStyleSel;
}

class CStyleSel : public QDialog
{
  Q_OBJECT

public:
  explicit CStyleSel(QWidget *parent = 0);
  ~CStyleSel();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_listWidget_doubleClicked(const QModelIndex &index);

  void on_pushButton_3_clicked();

private:
  Ui::CStyleSel *ui;
};

void loadQSSStyle(const QString &fileName = "");

#endif // CSTYLESEL_H

