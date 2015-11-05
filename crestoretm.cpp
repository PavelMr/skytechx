#include "crestoretm.h"
#include "ui_crestoretm.h"

typedef struct
{
  double  jd;
  QString desc;
  double  ra;
  double  dec;
} item_t;

class CTimeMarkItem : public QStandardItem
{
protected:
  bool operator< ( const QStandardItem & other ) const
  {
    item_t *i1 = (item_t *)data().toLongLong();
    item_t *i2 = (item_t *)other.data().toLongLong();

    double jd1 = i1->jd;
    double jd2 = i2->jd;

    return (jd1 < jd2);
  }
};

class CTimeMarkItemRD : public QStandardItem
{
protected:
  bool operator< ( const QStandardItem & other ) const
  {
    double a = data().toDouble();
    double b = other.data().toDouble();

    return a < b;
  }
};


CRestoreTM::CRestoreTM(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CRestoreTM)
{
  ui->setupUi(this);

  QStandardItemModel *m = new QStandardItemModel(0, 4);

  m->setHeaderData(0, Qt::Horizontal, QObject::tr("Date/Time"));
  m->setHeaderData(1, Qt::Horizontal, QObject::tr("Desc."));
  m->setHeaderData(2, Qt::Horizontal, QObject::tr("R.A."));
  m->setHeaderData(3, Qt::Horizontal, QObject::tr("Dec."));

  ui->treeView->sortByColumn(0);
  ui->treeView->setSortingEnabled(true);

  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/timemarks/timemarks.dat");
  QTextStream s(&f);

  int row = 0;
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QStringList list;

    while (!s.atEnd())
    {
      QString line = s.readLine();

      list = line.split(';');
      if (list.count() == 2 || list.count() == 4)
      {
        item_t *i = new item_t;

        i->desc = list.at(0);
        i->jd = list.at(1).toDouble();

        CTimeMarkItem *i0 = new CTimeMarkItem;
        QStandardItem *i1 = new QStandardItem;
        CTimeMarkItemRD *i2 = new CTimeMarkItemRD;
        CTimeMarkItemRD *i3 = new CTimeMarkItemRD;

        i0->setText(getStrDate(i->jd, 0) + " " + getStrTime(i->jd, 0));
        i0->setData((qint64)i);
        i1->setText(i->desc);

        if (list.count() == 4)
        {
          i->ra = D2R(list.at(2).toDouble());
          i->dec = D2R(list.at(3).toDouble());

          i2->setText(getStrRA(i->ra));
          i2->setData(i->ra);
          i3->setText(getStrDeg(i->dec));
          i3->setData(i->dec);
        }
        else
        {
          i->ra = CM_UNDEF;
          i->dec = CM_UNDEF;

          i2->setText(tr("N/A"));
          i2->setData(CM_UNDEF);
          i3->setText(tr("N/A"));
          i3->setData(CM_UNDEF);
        }

        m->setItem(row, 0, i0);
        m->setItem(row, 1, i1);
        m->setItem(row, 2, i2);
        m->setItem(row, 3, i3);

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
  item_t *e = (item_t *)item->data().toLongLong();
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
  item_t *e = (item_t *)item->data().toLongLong();

  m_jd = e->jd;
  m_ra = e->ra;
  m_dec = e->dec;

  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/timemarks/timemarks.dat");
  QTextStream s(&f);

  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    for (int i = 0; i < model->rowCount(); i++)
    {
      QStandardItem *item = model->item(i, 0);
      item_t *e = (item_t *)item->data().toLongLong();

      s << e->desc << ";" << QString::number(e->jd, 'f', 8);

      if (e->ra > CM_UNDEF)
      {
        s << ";" << R2D(e->ra) << ";" << R2D(e->dec);
      }

      s << "\n";

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
