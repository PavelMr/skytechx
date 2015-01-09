#ifndef CSEARCHWIDGET_H
#define CSEARCHWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

namespace Ui {
class CSearchWidget;
}

class CSearchWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CSearchWidget(QWidget *parent = 0);
  ~CSearchWidget();
  void setModel(QSortFilterProxyModel *model, int column);

signals:
  void sigSetSelection(QModelIndex &index);

private slots:
  void on_lineEdit_textChanged(const QString &arg1);

  void on_pb_search_clicked();

private:
  int findItem(const QString &text, int fromRow);

  Ui::CSearchWidget *ui;
  QSortFilterProxyModel *m_model;
  int                 m_row;
  QString             m_text;
  int                 m_column;
};

#endif // CSEARCHWIDGET_H
