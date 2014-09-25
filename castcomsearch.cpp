#include "castcomsearch.h"
#include "ui_castcomsearch.h"
#include "casterdlg.h"
#include "ccomdlg.h"

static bool bIsComet;
static bool g_sortIndex = 0;

bool CLWI_SAstCom::operator< ( const QListWidgetItem& other) const
{
  if (g_sortIndex == 0)
  {
    return(text() < other.text());
  }
  else
  {
    if (bIsComet)
    {
      comet_t *aa = (comet_t *)(data(Qt::UserRole).toInt());
      comet_t *b = (comet_t *)(other.data(Qt::UserRole).toInt());

      return(aa->orbit.mag < b->orbit.mag);
    }
    else
    {
      asteroid_t *a = (asteroid_t *)(data(Qt::UserRole).toInt());
      asteroid_t *b = (asteroid_t *)(other.data(Qt::UserRole).toInt());

      return(a->orbit.mag < b->orbit.mag);
    }
  }
}

CAstComSearch::CAstComSearch(QWidget *parent, double jd, bool isComet) :
  QDialog(parent),
  ui(new Ui::CAstComSearch)
{
  ui->setupUi(this);
  //setFixedSize(size());
  bIsComet = m_bComet = isComet;

  if (!isComet)
  {
    setWindowTitle(tr("Search a asteroid"));
    for (int i = 0; i < tAsteroids.count(); i++)
    {
      asteroid_t *a = &tAsteroids[i];

      if (!a->selected)
        continue;

      astSolve(a, jd);

      CLWI_SAstCom *witem = new CLWI_SAstCom;//(QString(a->name) + " " + getStrMag(a->orbit.mag), NULL, (int)a);

      witem->setText(QString(a->name) + " " + getStrMag(a->orbit.mag));
      witem->setData(Qt::UserRole, (int)a);
      ui->listWidget->addItem(witem);
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

      CLWI_SAstCom *witem = new CLWI_SAstCom;//(QString(a->name) + " " + getStrMag(a->orbit.mag), NULL, (int)a);

      witem->setText(QString(a->name) + " " + getStrMag(a->orbit.mag));
      witem->setData(Qt::UserRole, (int)a);
      ui->listWidget->addItem(witem);
    }
  }

  ui->comboBox->blockSignals(true);
  ui->comboBox->addItem(tr("Sort by name"));
  ui->comboBox->addItem(tr("Sort by magnitude"));
  ui->comboBox->blockSignals(false);
  ui->comboBox->setCurrentIndex(g_sortIndex);

  ui->listWidget->sortItems();
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
  QList <QListWidgetItem *> item = ui->listWidget->selectedItems();

  if (item.count() == 0)
    return;

  if (!m_bComet)
  {
    asteroid_t *a = (asteroid_t *)item[0]->data(Qt::UserRole).toInt();
    m_fov = DEG2RAD(AST_ZOOM);
    m_rd.Ra = a->orbit.lRD.Ra;
    m_rd.Dec = a->orbit.lRD.Dec;
  }
  else
  {
    comet_t *a = (comet_t *)item[0]->data(Qt::UserRole).toInt();
    m_fov = DEG2RAD(COM_ZOOM);
    m_rd.Ra = a->orbit.lRD.Ra;
    m_rd.Dec = a->orbit.lRD.Dec;
  }

  done(DL_OK);
}


////////////////////////////////////////////////////////////////////
void CAstComSearch::on_listWidget_doubleClicked(const QModelIndex &)
////////////////////////////////////////////////////////////////////
{
  on_pushButton_clicked();
}

//////////////////////////////////////////////////////////////
void CAstComSearch::on_comboBox_currentIndexChanged(int index)
//////////////////////////////////////////////////////////////
{
  g_sortIndex = index;
  ui->listWidget->sortItems();
}
