#include "skcore.h"
#include "ctracklist.h"
#include "ui_ctracklist.h"
#include "cobjtracking.h"

CTrackList::CTrackList(QWidget *parent, double tz) :
  QDialog(parent),
  ui(new Ui::CTrackList)
{
  ui->setupUi(this);
  setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint)
                    & ~Qt::WindowCloseButtonHint));

  QStandardItemModel *model = new QStandardItemModel(0, 4);

  model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  model->setHeaderData(1, Qt::Horizontal, tr("From"));
  model->setHeaderData(2, Qt::Horizontal, tr("To"));
  model->setHeaderData(3, Qt::Horizontal, tr("Steps"));

  for (int i = 0; i < tTracking.count(); i++)
  {
    QStandardItem *item = new QStandardItem;
    item->setText(tTracking[i].objName);
    item->setCheckable(true);
    if (tTracking[i].show)
    {
      item->setCheckState(Qt::Checked);
    }
    model->setItem(i, 0, item);

    item = new QStandardItem;
    item->setText(getStrDate(tTracking[i].jdFrom, tz) + " " + getStrTime(tTracking[i].jdFrom, tz, true));
    model->setItem(i, 1, item);

    item = new QStandardItem;
    item->setText(getStrDate(tTracking[i].jdTo, tz) + " " + getStrTime(tTracking[i].jdTo, tz, true));
    model->setItem(i, 2, item);

    item = new QStandardItem;
    item->setText(QString::number(tTracking[i].tPos.count()));
    model->setItem(i, 3, item);
  }

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->header()->resizeSection(0, 100);
  ui->treeView->header()->resizeSection(1, 120);
  ui->treeView->header()->resizeSection(2, 120);
  ui->treeView->header()->resizeSection(3, 60);

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDeleteItem()));
}

CTrackList::~CTrackList()
{
  delete ui;
}

void CTrackList::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

/////////////////////////////////
void CTrackList::slotDeleteItem()
/////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  model->removeRow(il.at(0).row());
  tTracking.removeAt(index);
}

////////////////////////////////////////
void CTrackList::on_pushButton_clicked()
////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    tTracking[i].show = item->checkState() ==  Qt::Checked ? true : false;
  }

  done(DL_OK);
}

//////////////////////////////////////////
void CTrackList::on_pushButton_3_clicked()
//////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  model->removeRows(0, model->rowCount());
  tTracking.clear();;
}
