#ifndef CSATELLITEDLG_H
#define CSATELLITEDLG_H

#include <QDialog>

namespace Ui {
class CSatelliteDlg;
}

class CSatelliteDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CSatelliteDlg(QWidget *parent = 0);
  ~CSatelliteDlg();

private:
  Ui::CSatelliteDlg *ui;
};

#endif // CSATELLITEDLG_H
