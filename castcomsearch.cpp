#include "castcomsearch.h"
#include "ui_castcomsearch.h"
#include "casterdlg.h"
#include "ccomdlg.h"

CAstComSearch::CAstComSearch(QWidget *parent, double jd, bool isComet) :
  QDialog(parent),
  ui(new Ui::CAstComSearch)
{
  ui->setupUi(this);
  m_bComet = isComet;

  if (!isComet)
  {
    setWindowTitle(tr("Search a asteroid"));
    for (int i = 0; i < tAsteroids.count(); i++)
    {
      asteroid_t *a = &tAsteroids[i];

      if (!a->selected)
        continue;

      astSolve(a, jd);

      ui->treeView->addRow(a->name, a->orbit.mag, (qint64)a);
    }
  }
  else
  {
    setWindowTitle(tr("Search a comet"));

    for (int i = 0; i < tComets.count(); i++)
    {
      comet_t *a = &tComets[i];

      if (!a->selected)
        continue;

      comSolve(a, jd);

      ui->treeView->addRow(a->name, a->orbit.mag, (qint64)a);
    }
  }

  ui->widget->setModel(ui->treeView->getModel(), 0);
  connect(ui->widget, SIGNAL(sigSetSelection(QModelIndex&)), this, SLOT(slotSelChange(QModelIndex&)));

  ui->treeView->setHeaderSize(180, 80);
}

CAstComSearch::~CAstComSearch()
{
  delete ui;
}

void CAstComSearch::changeEvent(QEvent *e)
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

void CAstComSearch::slotSelChange(QModelIndex &index)
{
  ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  ui->treeView->scrollTo(index);
}

/////////////////////////////////////////////
void CAstComSearch::on_pushButton_2_clicked()
/////////////////////////////////////////////
{
  done(DL_CANCEL);
}

///////////////////////////////////////////
void CAstComSearch::on_pushButton_clicked()
///////////////////////////////////////////
{
  QVariant data = ui->treeView->getSelectedData();

  if (!data.isValid())
  {
    return;
  }

  if (!m_bComet)
  {
    asteroid_t *a = (asteroid_t *)data.toLongLong();
    m_fov = DEG2RAD(AST_ZOOM);
    m_rd.Ra = a->orbit.lRD.Ra;
    m_rd.Dec = a->orbit.lRD.Dec;
  }
  else
  {
    comet_t *a = (comet_t *)data.toLongLong();
    m_fov = DEG2RAD(COM_ZOOM);
    m_rd.Ra = a->orbit.lRD.Ra;
    m_rd.Dec = a->orbit.lRD.Dec;
  }

  done(DL_OK);
}


void CAstComSearch::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_clicked();
}
