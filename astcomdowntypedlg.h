#ifndef ASTCOMDOWNTYPEDLG_H
#define ASTCOMDOWNTYPEDLG_H

#include <QDialog>

#define ACDT_UPDATE           0
#define ACDT_REMOVE           1
#define ACDT_ADD              2
#define ACDT_ADD_UPDATE       3

namespace Ui {
class AstComDownTypeDlg;
}

class AstComDownTypeDlg : public QDialog
{
  Q_OBJECT

public:
  explicit AstComDownTypeDlg(QWidget *parent = 0);
  ~AstComDownTypeDlg();

  int m_type;

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::AstComDownTypeDlg *ui;
};

#endif // ASTCOMDOWNTYPEDLG_H
