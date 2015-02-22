#include "crestoretm.h"
#include "ui_crestoretm.h"

typedef struct
{
  double  jd;
  QString desc;
} item_t;

class CTimeMarkItem : public QStandardItem
{
protected:
  bool	operator< ( const QStandardItem & other ) const
  {
    item_t *i1 = (item_t *)data().toInt();
    item_t *i2 = (item_t *)other.data().toInt();

    double jd1 = i1->jd;
    double jd2 = i2->jd;

    if (jd1 > jd2)
      return(true);

    return(false);
  }
};


CRestoreTM::CRestoreTM(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CRestoreTM)
{
  ui->setupUi(this);

  QStandardItemModel *m = new QStandardItemModel(0, 2);

  m->setHeaderData(0, Qt::Horizontal, QObject::tr("Date/Time"));
  m->setHeaderData(1, Qt::Horizontal, QObject::tr("Desc."));

  ui->treeView->sortByColumn(0);
  ui->treeView->setSortingEnabled(true);

  SkFile f("data/timemarks/timemarks.dat");
  QTextStream s(&f);

  int row = 0;
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QStringList list;

    while (!s.atEnd())
    {
      QString line = s.readLine();

      list = line.split(';');
      if (list.count() == 2)
      {
        item_t *i = new item_t;

        i->desc = list.at(0);
        i->jd = list.at(1).toDouble();

        CTimeMarkItem *i0 = new CTimeMarkItem;
        QStandardItem *i1 = new QStandardItem;

        i0->setText(getStrDate(i->jd, 0) + " " + getStrTime(i->jd, 0));
        i0->setData((int)i);
        i1->setText(i->desc);

        m->setItem(row, 0, i0);
        m->setItem(row, 1, i1);

        row++;
      }
    }
  }

  ui->treeView->setModel(m);
  ui->treeView->header()->resizeSection(0, 140);

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDelete()));
}

CRestoreTM::~CRestoreTM()
{
  delete ui;
}

void CRestoreTM::changeEvent(QEvent *e)
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

/////////////////////////////
void CRestoreTM::slotDelete()
/////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();

  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  item_t *e = (item_t *)item->data().toInt();
  delete e;
  model->removeRow(il.at(0).row());
}

//////////////////////////////////////////
void CRestoreTM::on_pushButton_2_clicked()
//////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();

  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  item_t *e = (item_t *)item->data().toInt();

  m_jd = e->jd;

  SkFile f("data/timemarks/timemarks.dat");
  QTextStream s(&f);

  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    for (int i = 0; i < model->rowCount(); i++)
    {
      QStandardItem *item = model->item(i, 0);
      item_t *e = (item_t *)item->data().toInt();

      s << e->desc << ";" << QString::number(e->jd, 'f', 8) << "\n";

      delete e;
    }
  }

  model->removeRows(0, model->rowCount());

  done(DL_OK);
}

///////////////////////////////////////////////////////////////
void CRestoreTM::on_treeView_doubleClicked(const QModelIndex &)
///////////////////////////////////////////////////////////////
{
  on_pushButton_2_clicked();
}


////////////////////////////////////////
void CRestoreTM::on_pushButton_clicked()
////////////////////////////////////////
{
  done(DL_CANCEL);
}

void CRestoreTM::on_pushButton_3_clicked()
{
  slotDelete();
}
