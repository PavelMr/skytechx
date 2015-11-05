#include "cinsertfinder.h"
#include "ui_cinsertfinder.h"

#include "skcore.h"
#include "cfinderedit.h"

CInsertFinder::CInsertFinder(QWidget *parent) :
  QDialog(parent),
  m_fov(0.1),
  m_text("???"),
  ui(new Ui::CInsertFinder)
{
  ui->setupUi(this);

  load();

  connect(ui->listWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex ,QModelIndex)), this, SLOT(slotIndexChanged(QModelIndex,QModelIndex)));

  if (ui->listWidget->count() > 0)
  {
    ui->listWidget->setSelectionIndex(0);
  }

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidget, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(deleteItem()));
}

CInsertFinder::~CInsertFinder()
{
  for (int i = 0; i < ui->listWidget->count(); i++)
  {
    finderScope_t *f = (finderScope_t *)ui->listWidget->getCustomData(i).toLongLong();

    delete f;
  }


  delete ui;
}

void CInsertFinder::load()
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope/finder.dat");

  if (f.open(QFile::Text | QFile::ReadOnly))
  {
    QString     str;
    QStringList list;
    do
    {
      str = f.readLine();
      if (str.isEmpty() || str.startsWith("\n"))
      {
        break;
      }

      list = str.split("|");
      if (list.count() != 3)
      {
        qDebug("load (finderscope) read line fail!");
        continue;
      }

      finderScope_t *b = new finderScope_t;

      b->name = list[0].simplified();
      b->fov = list[1].toDouble();
      b->magnification = list[2].toDouble();

      ui->listWidget->addRow(list[0], (qint64)b);
    } while (1);
  }
}

void CInsertFinder::save()
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope/finder.dat");

  if (f.open(QFile::Text | QFile::WriteOnly))
  {
    QTextStream ts(&f);

    for (int i = 0; i < ui->listWidget->count(); i++)
    {
      finderScope_t *b = (finderScope_t *)ui->listWidget->getCustomData(i).toLongLong();

      ts << b->name << "|"  << b->fov << "|" << b->magnification << "\n";
    }
  }
}

void CInsertFinder::slotIndexChanged(QModelIndex current, QModelIndex /*prev*/)
{
  int currentRow = current.row();

  if (currentRow == -1)
  {
    ui->label_p1->setText("?");
    ui->label_p2->setText("?");
    return;
  }

  finderScope_t *b = (finderScope_t *)ui->listWidget->getCustomData(currentRow).toLongLong();

  ui->label_p1->setText(QString("%1°").arg(b->fov, 0, 'f', 2));
  ui->label_p2->setText(QString("%1x").arg(b->magnification, 0, 'f', 2));
}

void CInsertFinder::deleteItem()
{
  int i = ui->listWidget->getSelectionIndex();

  if (i == -1)
  {
    return;
  }

  finderScope_t *f = (finderScope_t *)ui->listWidget->getCustomData(i).toLongLong();

  delete f;
  ui->listWidget->removeAt(i);
}

void CInsertFinder::on_pushButton_2_clicked()
{
  int currentRow = ui->listWidget->getSelectionIndex();

  if (currentRow == -1)
  {
    return;
  }

  finderScope_t *f = (finderScope_t *)ui->listWidget->getCustomData(currentRow).toLongLong();

  m_text = f->name;
  m_fov = D2R(f->fov);

  save();

  done(DL_OK);
}

void CInsertFinder::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CInsertFinder::on_listWidget_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}

void CInsertFinder::on_pushButton_4_clicked()
{
  deleteItem();
}

void CInsertFinder::on_pushButton_3_clicked()
{
  CFinderEdit dlg(this, true);

  if (dlg.exec() == DL_OK)
  {
    finderScope_t *b = new finderScope_t;

    b->name = dlg.m_name;
    b->fov = dlg.m_fov;
    b->magnification = dlg.m_mag;

    ui->listWidget->addRow(b->name, (qint64)b);
  }
}

void CInsertFinder::on_pushButton_5_clicked()
{
  int i = ui->listWidget->getSelectionIndex();

  if (i == -1)
  {
    return;
  }

  finderScope_t *b = (finderScope_t *)ui->listWidget->getCustomData(i).toLongLong();
  CFinderEdit dlg(this, false, b->name, b->magnification, b->fov);

  if (dlg.exec() == DL_OK)
  {
    b->name = dlg.m_name;
    b->magnification = dlg.m_mag;
    b->fov = dlg.m_fov;

    ui->listWidget->setTextAt(i, b->name);
    slotIndexChanged(ui->listWidget->currentIndex(), QModelIndex());
  }
}
