#include "cdrawinglist.h"
#include "ui_cdrawinglist.h"
#include "cdrawing.h"
#include "skcore.h"

CDrawingList::CDrawingList(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDrawingList)
{
  ui->setupUi(this);

  setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint)
                 & ~Qt::WindowCloseButtonHint));

  m_model = new QStandardItemModel(0, 2);

  m_model->setHeaderData(0, Qt::Horizontal, tr("Type"));
  m_model->setHeaderData(1, Qt::Horizontal, tr("Description"));

  drawing_t *draw;

  for (int i = 0; ; i++)
  {
    draw = getDrawing(i);
    if (draw == NULL)
    {
      break;
    }

    QStandardItem *item = new QStandardItem;
    QStandardItem *item1 = new QStandardItem;

    item->setCheckable(true);
    if (draw->show)
    {
      item->setCheckState(Qt::Checked);
    }

    switch (draw->type)
    {
      case DT_TELESCOPE:
        item->setText(tr("Telescope"));
        item1->setText(draw->telescope_t.name);
        break;

      case DT_TELRAD:
        item->setText(tr("Telrad"));
        item1->setText("Telrad finderscope");
        break;

      case DT_TEXT:
        item->setText(tr("Text"));
        break;

      case DT_FRM_FIELD:
        item->setText(tr("Frame field"));
        item1->setText(QString("%1' x %2'").arg(R2D(draw->frmField_t.x * 60), 0, 'f', 2).arg(R2D(draw->frmField_t.y * 60), 0, 'f', 2));
        break;
    }
    item->setData((int)draw);
    m_model->setItem(i, 0, item);
    m_model->setItem(i, 1, item1);
  }

  ui->treeView->setRootIsDecorated(false);
  ui->treeView->header()->resizeSection(0, 100);
  ui->treeView->header()->resizeSection(1, 300);
  ui->treeView->setModel(m_model);

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDelete()));
}

CDrawingList::~CDrawingList()
{
  delete ui;
}

void CDrawingList::changeEvent(QEvent *e)
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


void CDrawingList::on_pushButton_2_clicked()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  drawing_t *draw = (drawing_t *)il.at(0).data(Qt::UserRole + 1).toInt();

  m_ra = draw->rd.Ra;
  m_dec = draw->rd.Dec;
  m_fov = D2R(20);

  done(DL_OK);
}

void CDrawingList::slotDelete()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  drawing_t *draw = (drawing_t *)il.at(0).data(Qt::UserRole + 1).toInt();

  deleteDrawing(draw);

  m_model->removeRow(il.at(0).row());
}

void CDrawingList::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}

void CDrawingList::on_pushButton_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    drawing_t *draw = (drawing_t *)item->data(Qt::UserRole + 1).toInt();

    draw->show = item->checkState() ==  Qt::Checked ? true : false;
  }

  done(DL_CANCEL);
}

void CDrawingList::on_pushButton_3_clicked()
{
  slotDelete();
}
