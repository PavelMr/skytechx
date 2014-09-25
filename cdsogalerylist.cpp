#include "skcore.h"
#include "cdsogalerylist.h"
#include "ui_cdsogalerylist.h"
#include "cdb.h"
#include "cgalery.h"

#include <QStandardItemModel>

CDSOGaleryList::CDSOGaleryList(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDSOGaleryList)
{
  ui->setupUi(this);

  QStandardItemModel *m = new QStandardItemModel;

  m->setColumnCount(2);
  m->setHeaderData(0, Qt::Horizontal, tr("Name"));
  m->setHeaderData(1, Qt::Horizontal, tr("Images count"));

  ui->treeView->setModel(m);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setColumnWidth(0, 200);

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDelete()));

  fillList();
}

CDSOGaleryList::~CDSOGaleryList()
{
  delete ui;
}

void CDSOGaleryList::changeEvent(QEvent *e)
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

void CDSOGaleryList::fillList()
{
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

  m->removeRows(0, m->rowCount());

  QSqlQuery q(*g_pDb);

  q.exec("select name,common_name,COUNT(*) as count from gallery GROUP BY common_name;");

  int nameNo = q.record().indexOf("common_name");
  int countNo = q.record().indexOf("count");
  int idNo = q.record().indexOf("name");

  while (q.next())
  {
    QString name = q.value(nameNo).toString();
    int count = q.value(countNo).toInt();
    QString id = q.value(idNo).toString();

    QList <QStandardItem *> row;
    QStandardItem *i1 = new QStandardItem;
    QStandardItem *i2 = new QStandardItem;

    i1->setText(name);
    i1->setData(name);
    i2->setText(QString::number(count));
    i2->setData(id);

    row.append(i1);
    row.append(i2);

    m->appendRow(row);
  }
}

void CDSOGaleryList::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void CDSOGaleryList::on_pushButton_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
  {
    return;
  }

  QStandardItem *item1 = model->itemFromIndex(il.at(0));
  QStandardItem *item2 = model->itemFromIndex(il.at(1));

  CGalery dlg(this, item2->data().toString(), item1->data().toString());
  dlg.exec();

  fillList();
}

void CDSOGaleryList::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_clicked();
}

void CDSOGaleryList::on_pushButton_3_clicked()
{
   if (msgBoxQuest(this, tr("Remove all images?")) == QMessageBox::No)
   {
     return;
   }

   QSqlQuery q(*g_pDb);

   q.exec("delete from gallery;");
   q.finish();

   fillList();
}

void CDSOGaleryList::slotDelete()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
  {
    return;
  }

  QStandardItem *item1 = model->itemFromIndex(il.at(0));
  QStandardItem *item2 = model->itemFromIndex(il.at(1));

  QSqlQuery q(*g_pDb);

  q.exec(QString("delete from gallery where name = '%1';").arg(item2->data().toString()));
  q.finish();

  model->removeRow(item1->row());

  qDebug() << "del";
}
