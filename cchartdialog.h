#ifndef CCHARTDIALOG_H
#define CCHARTDIALOG_H

#include <QDialog>
#include <QtCharts/QChartView>

class QPrinter;

namespace Ui {
class CChartDialog;
}

QT_CHARTS_USE_NAMESPACE

class MyChartView : public QChartView
{
  Q_OBJECT
public:
  MyChartView(QChart *chart);

protected:
  void mouseMoveEvent(QMouseEvent *e);  

signals:
  void sigChartValue(const QDateTime &dt, double Y1, double Y2, bool isY2);
};

class CChartDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CChartDialog(QWidget *parent, const QString &object, QList<QPair<double, double> > &chart1, QList<QPair<double, double> > &chart2,
                        const QString &name1, const QString &name2, bool invertY1, bool invertY2);
  ~CChartDialog();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void slotPrintPreview(QPrinter *printer);

  void on_pushButton_4_clicked();

  void slotChartValue(const QDateTime &dt, double Y1, double Y2, bool isY2);

private:
  Ui::CChartDialog *ui;
  QChartView *m_chartView;
  bool m_invertY1;
  bool m_invertY2;
};

#endif // CCHARTDIALOG_H
