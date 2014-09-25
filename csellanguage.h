#ifndef CSELLANGUAGE_H
#define CSELLANGUAGE_H

#include <QDialog>

namespace Ui {
class CSelLanguage;
}

class CSelLanguage : public QDialog
{
  Q_OBJECT

public:
  explicit CSelLanguage(QWidget *parent = 0);
  ~CSelLanguage();

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_listWidget_doubleClicked(const QModelIndex &);

private:
  Ui::CSelLanguage *ui;
};

#endif // CSELLANGUAGE_H
