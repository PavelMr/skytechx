#ifndef CDSSMANAGER_H
#define CDSSMANAGER_H

#include <QDialog>
#include <QResizeEvent>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>

class CManagerModel : public QSortFilterProxyModel
{
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const
  { // sorting
    if (sortColumn() == 1)
    {
      return (left.data(Qt::UserRole + 1).toLongLong() < right.data(Qt::UserRole + 1).toLongLong());
    }

    if (sortColumn() == 2 || sortColumn() == 3)
    {
      return (left.data(Qt::UserRole + 1).toDouble() < right.data(Qt::UserRole + 1).toDouble());
    }

    if (sortColumn() == 4)
    {
      return (left.data(Qt::UserRole + 1).toDateTime() < right.data(Qt::UserRole + 1).toDateTime());
    }

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
  }
};

namespace Ui {
class CDSSManager;
}

class CDSSManager : public QDialog
{
  Q_OBJECT

public:
  explicit CDSSManager(QWidget *parent = 0);
  ~CDSSManager();

protected:
  void resizeEvent(QResizeEvent *e);

private slots:
  void on_pushButton_2_clicked();
  void on_pushButton_3_clicked();
  void slotDelete();

  void on_pushButton_clicked();

private:
  Ui::CDSSManager *ui;

  void fillList();
  void updateGui();

  QStandardItemModel *m_model;
  CManagerModel *m_proxy;
  qint64 m_totalSize;
};

#endif // CDSSMANAGER_H
