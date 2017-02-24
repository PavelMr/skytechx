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

  m_model = new QStandardItemModel(0, 3);

  m_model->setHeaderData(0, Qt::Horizontal, tr("Type"));
  m_model->setHeaderData(1, Qt::Horizontal, tr("LWT"));
  m_model->setHeaderData(1, Qt::Horizontal, tr("Linked with telescope"), Qt::ToolTipRole);
  m_model->setHeaderData(2, Qt::Horizontal, tr("Description"));

  drawing_t *draw;

  for (int i = 0; ; i++)
  {
    draw = getDrawing(i);
    if (draw == NULL)
    {
      break;
    }

    QStandardItem *item = new QStandardItem;
    QStandardItem *item2 = new QStandardItem;
    QStandardItem *item1 = new QStandardItem;

    item->setCheckable(true);
    if (draw->show)
    {
      item->setCheckState(Qt::Checked);
    }

    item2->setToolTip(tr("Linked with telescope"));
    item2->setCheckable(true);
    if (draw->telescopeLink)
    {
      item2->setCheckState(Qt::Checked);
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
        item1->setText(draw->text_t.text);
        break;

      case DT_FRM_FIELD:
        item->setText(tr("Frame field"));
        item1->setText(QString("%1' x %2'").arg(R2D(draw->frmField_t.x * 60), 0, 'f', 2).arg(R2D(draw->frmField_t.y * 60), 0, 'f', 2));
        break;

      case DT_EXT_FRAME:
        item->setText(tr("External Frame field"));
        item1->setText(QString("%1' x %2'").arg(R2D(draw->frmField_t.x * 60), 0, 'f', 2).arg(R2D(draw->frmField_t.y * 60), 0, 'f', 2));
        break;
    }
    item->setData((qint64)draw);
    m_model->setItem(i, 0, item);
    m_model->setItem(i, 1, item2);
    m_model->setItem(i, 2, item1);
  }

  ui->treeView->setModel(m_model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->header()->resizeSection(0, 150);
  ui->treeView->header()->resizeSection(1, 60);
  ui->treeView->header()->resizeSection(2, 200);


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

  drawing_t *draw = (drawing_t *)il.at(0).data(Qt::UserRole + 1).toLongLong();

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

  drawing_t *draw = (drawing_t *)il.at(0).data(Qt::UserRole + 1).toLongLong();

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
    QStandardItem *item2 = model->item(i, 1);

    drawing_t *draw = (drawing_t *)item->data(Qt::UserRole + 1).toLongLong();

    draw->show = item->checkState() ==  Qt::Checked ? true : false;
    draw->telescopeLink = item2->checkState() ==  Qt::Checked ? true : false;
  }

  done(DL_CANCEL);
}

void CDrawingList::on_pushButton_3_clicked()
{
  slotDelete();
}
