#ifndef CHORIZONEDITOR_H
#define CHORIZONEDITOR_H

#include <QDialog>
#include <QFrame>
#include <QWidget>

namespace Ui {
class CHorizonEditor;
}

class CHorizonEditor : public QDialog
{
  Q_OBJECT

public:
  explicit CHorizonEditor(QWidget *parent = 0);
  ~CHorizonEditor();

private slots:
  void on_lineEdit_textChanged(const QString &arg1);

  void on_pushButton_clicked();

  void on_tabWidget_currentChanged(int index);

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

private:
  Ui::CHorizonEditor *ui;
};

#endif // CHORIZONEDITOR_H
