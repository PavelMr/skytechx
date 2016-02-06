#include "cdssmanager.h"
#include "ui_cdssmanager.h"
#include "cfits.h"
#include "cbkimages.h"
#include "cimagemanip.h"

#include <QDir>

CDSSManager::CDSSManager(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDSSManager),
  m_totalSize(0)
{
  ui->setupUi(this);

  m_model = new QStandardItemModel(0, 6, NULL);
  m_proxy = new CManagerModel();
  m_proxy->setSourceModel(m_model);

  m_model->setHeaderData(0, Qt::Horizontal, QObject::tr("File name"));
  m_model->setHeaderData(1, Qt::Horizontal, QObject::tr("File size"));
  m_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Dimension"));
  m_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Size"));
  m_model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date"));
  m_model->setHeaderData(5, Qt::Horizontal, QObject::tr("Loaded"));

  //ui->treeView->header()->resizeSection(5, 14);
  //ui->treeView->resizeColumnToContents(0);

  ui->treeView->setModel(m_proxy);

  fillList();

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDelete()));
}

CDSSManager::~CDSSManager()
{
  delete m_proxy;
  delete ui;
}

void CDSSManager::resizeEvent(QResizeEvent *)
{
  for (int i = 0; i < 4; i++)
  {
    ui->treeView->resizeColumnToContents(i);
    ui->treeView->header()->resizeSection(i, ui->treeView->header()->sectionSize(i) + 16);
  }
}

void CDSSManager::fillList()
{
  m_model->removeRows(0, m_model->rowCount());

  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/", "*.fits");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.count(); i++)
  {
    bool memOk;

    QFileInfo fi = list.at(i);

    CFits *f = new CFits;
    if (!f->load(fi.filePath(), memOk, false))
    {
      delete f;
    }
    else
    {
      QStandardItem *item1 = new QStandardItem;
      QStandardItem *item2 = new QStandardItem;
      QStandardItem *item3 = new QStandardItem;
      QStandardItem *item4 = new QStandardItem;
      QStandardItem *item5 = new QStandardItem;
      QStandardItem *item6 = new QStandardItem;

      double size = anSep(f->m_cor[0].Ra, f->m_cor[0].Dec, f->m_cor[2].Ra, f->m_cor[2].Dec);

      item1->setText(f->m_name);
      item1->setData(fi.filePath());

      item2->setData((qlonglong)fi.size());
      item2->setText(QString("%1 MB").arg(fi.size() / 1024.0 / 1024.0, 0, 'f', 2));

      item3->setText(QString("%1'").arg(R2D(size / sqrt(2.0) * 60.0), 0, 'f', 1));
      item3->setData((double)R2D(size / sqrt(2.0) * 60.0));

      item4->setText(QString("%1 x %2").arg(f->m_xSize).arg(f->m_ySize));
      item4->setData((double)f->m_xSize);

      item5->setText(fi.lastModified().toString(Qt::SystemLocaleShortDate));
      item5->setData(fi.lastModified());

      item6->setData(0);

      for (int j = 0; j < bkImg.m_tImgList.count(); j++)
      {
        if (bkImg.m_tImgList[j].fileName == f->m_name)
        {
          item6->setIcon(QIcon(":/res/ico_green.png"));
          item6->setData(1);
          break;
        }
      }

      QList <QStandardItem *> list({item1, item2, item3, item4, item5, item6});

      m_model->appendRow(list);

      m_totalSize += fi.size();

      delete f;
    }
  }

  updateGui();
}

void CDSSManager::updateGui()
{
  ui->label_2->setText(QString("%1 MB").arg(m_totalSize / 1024.0 / 1024.0, 0, 'f', 2));
}

void CDSSManager::on_pushButton_2_clicked()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
  {
    return;
  }

  QModelIndex index = m_proxy->mapToSource(il.at(0));
  QStandardItem *item = m_model->item(index.row(), 0);

  if (bkImg.load(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/" + item->text()))
  {
    QStandardItem *item = m_model->item(index.row(), 5);

    item->setIcon(QIcon(":/res/ico_green.png"));
  }
}

void CDSSManager::on_pushButton_3_clicked()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
  {
    return;
  }

  QModelIndex index = m_proxy->mapToSource(il.at(0));
  QStandardItem *item1 = m_model->item(index.row(), 0);
  QStandardItem *item2 = m_model->item(index.row(), 1);

  m_totalSize -= item2->data().toLongLong();

  QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/" + item1->text());
  m_model->removeRow(index.row());
  updateGui();
}

void CDSSManager::slotDelete()
{
  on_pushButton_3_clicked();
}

void CDSSManager::on_pushButton_clicked()
{
  done(0);
}

