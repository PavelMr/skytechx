#ifndef CDSOCATALOGUE_H
#define CDSOCATALOGUE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "castro.h"
#include "cmapview.h"
#include "dso_def.h"
#include "caddcustomobject.h"

extern CMapView   *pcMapView;

class MyProxyDSOModel: public QSortFilterProxyModel
{
public:
  MyProxyDSOModel()
  {
    m_catalogue = NULL;
    m_catalogueIndex = -1;
    m_objectType = -1;
    m_empty = true;
    m_magLimitEnabled = false;
    m_sizeLimitType = 0;
    m_nameFilter = QString("");
  }

  void setMagLimit(bool enabled, double mag)
  {
    m_magLimit = mag;
    m_magLimitEnabled = enabled;
  }

  void setSizeLimit(int type, double size)
  {
    m_sizeLimit = size;
    m_sizeLimitType = type;
  }

  void setObjectType(int type)
  {
    m_objectType = type;
  }

  void setCatalogue(int index, QList <customCatalogue_t> *catalogue)
  {
    m_catalogue = catalogue;
    m_catalogueIndex = index;
  }

  void setNameFilter(QString name)
  {
    m_nameFilter = name;
  }

  void setConstFilter(QString name)
  {
    m_constFilter = name;
  }

  void setOnScreenFilter(bool enable)
  {
    m_onScreenOnly = enable;
  }

  void setLimits(double fromRa, double toRa, double fromDec, double toDec)
  {
    m_fromRa = fromRa;
    m_toRa = toRa;
    m_fromDec = fromDec;
    m_toDec = toDec;
  }

  void setFiltering()
  {
    m_empty = false;
  }

  void setAboveHorOnly(bool enable, CAstro *astro)
  {
    m_aboveHor = enable;
    m_astro = astro;
  }

  void beginReset() { beginResetModel(); }
  void endReset() { endResetModel(); }

private:
  CAstro *m_astro;
  bool   m_aboveHor;
  double m_fromRa, m_toRa, m_fromDec, m_toDec;
  bool m_onScreenOnly;
  int m_objectType;
  bool m_empty;
  bool m_magLimitEnabled;
  double m_magLimit;
  int m_sizeLimitType;
  double m_sizeLimit;
  QString m_nameFilter;
  QString m_constFilter;
  int m_catalogueIndex;
  QList <customCatalogue_t> *m_catalogue;

protected:
   bool lessThan(const QModelIndex& left, const QModelIndex& right) const
   { // sorting
     if (sortColumn() == 5 ||
         sortColumn() == 6 ||
         sortColumn() == 7 ||
         sortColumn() == 8 ||
         sortColumn() == 9 ||
         sortColumn() == 11)
     {
       return (left.data(Qt::UserRole + 1).toDouble() < right.data(Qt::UserRole + 1).toDouble());
     }

     QVariant leftData = sourceModel()->data(left);
     QVariant rightData = sourceModel()->data(right);

     return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
   }

   virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
   { // filtering
      if (m_empty)
      {
        return false;
      }

      QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

      int dsoIndex = index0.data(Qt::UserRole + 2).toInt();

      if (m_catalogueIndex >= 0)
      {
        if (!m_catalogue->at(m_catalogueIndex).list.contains(dsoIndex))
        {
          return false;
        }
      }

      QModelIndex index12 = sourceModel()->index(sourceRow, 12, sourceParent);
      QModelIndex index7 = sourceModel()->index(sourceRow, 7, sourceParent);
      QModelIndex index8 = sourceModel()->index(sourceRow, 8, sourceParent);
      QModelIndex index9 = sourceModel()->index(sourceRow, 9, sourceParent);
      QModelIndex index4 = sourceModel()->index(sourceRow, 4, sourceParent);

      QModelIndex index5 = sourceModel()->index(sourceRow, 5, sourceParent); // ra
      QModelIndex index6 = sourceModel()->index(sourceRow, 6, sourceParent); // dec

      int type = index0.data(Qt::UserRole + 1).toInt();

      double ra = index5.data(Qt::UserRole + 1).toDouble();
      double dec = index6.data(Qt::UserRole + 1).toDouble();

      if (m_onScreenOnly && !pcMapView->isRaDecOnScreen(ra, dec))
      {
        return false;
      }

      if (ra < m_fromRa || ra > m_toRa)
      {
        return false;
      }

      if (dec > m_fromDec || dec < m_toDec)
      {
        return false;
      }

      if (m_aboveHor)
      {
        double alt, azm;

        m_astro->convRD2AARef(ra, dec, &azm, &alt);

        if (alt <= 0)
        {
          return false;
        }
      }

      if (type == DSOT_NGC_DUPP)
      {
        return false;
      }

      if (m_objectType != -1 && type != m_objectType)
      {
        return false;
      }

      if (m_magLimitEnabled && index9.data(Qt::UserRole + 1).toDouble() > m_magLimit)
      {
        return false;
      }

      if (m_sizeLimitType > 0)
      {
        if (m_sizeLimitType == 1 && qMax(index7.data(Qt::UserRole + 1).toDouble(), index8.data(Qt::UserRole + 1).toDouble()) > m_sizeLimit)
        {
          return false;
        }
        else
        if (m_sizeLimitType == 2 && qMax(index7.data(Qt::UserRole + 1).toDouble(), index8.data(Qt::UserRole + 1).toDouble()) < m_sizeLimit)
        {
          return false;
        }
      }

      if (!m_nameFilter.isEmpty() && !index12.data().toString().contains(m_nameFilter, Qt::CaseInsensitive))
      {
        return false;
      }

      if (!m_constFilter.isEmpty() && !index4.data().toString().contains(m_constFilter, Qt::CaseInsensitive))
      {
        return false;
      }

      return true;
   }
};

namespace Ui {
class CDSOCatalogue;
}

class CDSOCatalogue : public QDialog
{
  Q_OBJECT

public:
  explicit CDSOCatalogue(QWidget *parent, mapView_t *view);
  ~CDSOCatalogue();
  int m_selectedIndex;

protected:
  void changeEvent(QEvent *e);
  void fillList();
  void updateCount();

private slots:
  void on_pushButton_2_clicked();

  void on_checkBox_toggled(bool checked);

  void on_sizeComboBox_currentIndexChanged(int index);

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

  void on_tableView_doubleClicked(const QModelIndex &index);

  void on_cb_top_toggled(bool checked);

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_8_clicked();

signals:
  void sigCenterObject();

private:
  CAstro m_astro;
  Ui::CDSOCatalogue *ui;
  mapView_t *m_mapView;
  MyProxyDSOModel* m_proxy;
  QStandardItemModel* m_model;
  QList <customCatalogue_t> m_catalogue;
};

#endif // CDSOCATALOGUE_H
