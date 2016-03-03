#ifndef CCHARTDIALOG_H
#define CCHARTDIALOG_H

#include <QDialog>

class QPrinter;

namespace Ui {
class CChartDialog;
}

class CChartDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CChartDialog(QWidget *parent, const QString &object, QList<QPair<double, double> > &chart1, QList<QPair<double, double> > &chart2, const QString &name1, const QString &name2);
  ~CChartDialog();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void slotPrintPreview(QPrinter *printer);

private:
  Ui::CChartDialog *ui;
};

#endif // CCHARTDIALOG_H
