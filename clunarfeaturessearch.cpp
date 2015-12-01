#include "clunarfeaturessearch.h"
#include "ui_clunarfeaturessearch.h"
#include "clunarfeatures.h"
#include "skcore.h"

#include <QDebug>

CLunarFeaturesSearch::CLunarFeaturesSearch(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CLunarFeaturesSearch)
{
  ui->setupUi(this);

  m_view = view;
  m_model = new QStandardItemModel(0, 5);

  m_proxy = new MyProxyLFModel();
  m_proxy->setSourceModel(m_model);

  m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  m_model->setHeaderData(1, Qt::Horizontal, tr("Type"));
  m_model->setHeaderData(2, Qt::Horizontal, tr("Diameter"));
  m_model->setHeaderData(3, Qt::Horizontal, tr("Longitude"));
  m_model->setHeaderData(4, Qt::Horizontal, tr("Latitude"));

  int i = 0;
  foreach (const lunarItem_t &lf, cLunarFeatures.tLunarItems)
  {
    bool b = cLunarFeatures.isVisible(i, m_view);

    QStandardItem *item = new QStandardItem;
    item->setText(lf.name);
    item->setData(i);
    QFont fnt = item->font();
    fnt.setBold(b);
    item->setFont(fnt);
    m_model->setItem(i, 0, item);

    QStandardItem *item1 = new QStandardItem;
    item1->setText(CLunarFeatures::getTypeName(lf.type));
    item1->setData(b);
    m_model->setItem(i, 1, item1);

    QStandardItem *item2 = new QStandardItem;
    item2->setText(QString::number(lf.rad) + tr(" km"));
    item2->setData(lf.rad);
    m_model->setItem(i, 2, item2);

    QStandardItem *item3 = new QStandardItem;
    item3->setText(getStrLon(lf.lon));
    item3->setData(lf.lon);
    m_model->setItem(i, 3, item3);

    QStandardItem *item4 = new QStandardItem;
    item4->setText(getStrLat(lf.lat));
    item4->setData(lf.lat);
    m_model->setItem(i, 4, item4);

    i++;
  }

  ui->treeView->setModel(m_proxy);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->header()->resizeSection(0, 200);
  ui->treeView->header()->resizeSection(1, 100);
  ui->treeView->header()->resizeSection(2, 80);
  ui->treeView->header()->resizeSection(3, 120);
  ui->treeView->header()->resizeSection(4, 120);
  ui->treeView->setSortingEnabled(true);

  ui->treeView->sortByColumn(0, Qt::AscendingOrder);
}

CLunarFeaturesSearch::~CLunarFeaturesSearch()
{
  delete ui;
}

void CLunarFeaturesSearch::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CLunarFeaturesSearch::on_pushButton_2_clicked()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
  {
    return;
  }

  QModelIndex index = m_proxy->mapToSource(il.at(0));
  QStandardItem *item = m_model->item(index.row(), 0);
  QStandardItem *item1 = m_model->item(index.row(), 1);
  int lfIndex = item->data().toInt();
  int vis = item1->data().toInt();

  if (!vis)
  {
    msgBoxInfo(this, tr("This lunar feature is not visible!"));
    return;
  }

  Q_ASSERT(cLunarFeatures.search("", m_view, m_rd.Ra, m_rd.Dec, m_fov, lfIndex));

  done(DL_OK);
}


void CLunarFeaturesSearch::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}
