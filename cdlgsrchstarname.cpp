#include "cdlgsrchstarname.h"
#include "ui_cdlgsrchstarname.h"

static int g_sortIndex = 0;


bool CLWI_SStars::operator< ( const QListWidgetItem & other ) const
{
  if (g_sortIndex == 0)
    return(text() < other.text());
  else
  {
    tychoStar_t *t1 = (tychoStar_t *)(data(Qt::UserRole).toInt());
    tychoStar_t *t2 = (tychoStar_t *)(other.data(Qt::UserRole).toInt());

    double m1 = cTYC.getVisMag(t1);
    double m2 = cTYC.getVisMag(t2);

    return(m1 < m2);
  }
}


CDlgSrchStarName::CDlgSrchStarName(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDlgSrchStarName)
{
  ui->setupUi(this);
  //setFixedSize(size());

  for (int i = 0; i < cTYC.tNames.count(); i++)
  {
    int offs = cTYC.tNames[i]->supIndex;
    QString name = cTYC.getStarName(&cTYC.pSupplement[offs]);

    CLWI_SStars *item = new CLWI_SStars;

    item->setText(name);
    item->setData(Qt::UserRole, (int)cTYC.tNames[i]);

    ui->listWidget->addItem(item);
  }

  ui->comboBox->addItem(tr("Sort in Alphabetical order"));
  ui->comboBox->addItem(tr("Sort by magnitude"));
  ui->comboBox->setCurrentIndex(g_sortIndex);

  connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sort()));

  sort();
}

CDlgSrchStarName::~CDlgSrchStarName()
{
  delete ui;
}

void CDlgSrchStarName::changeEvent(QEvent *e)
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
void CDlgSrchStarName::sort()
/////////////////////////////
{
  g_sortIndex = ui->comboBox->currentIndex();
  ui->listWidget->sortItems();
}

//////////////////////////////////////////////
void CDlgSrchStarName::on_pushButton_clicked()
//////////////////////////////////////////////
{
  QList <QListWidgetItem *> item = ui->listWidget->selectedItems();

  if (item.count() == 0)
    return;

  m_tycho = (tychoStar_t *)(item[0]->data(Qt::UserRole).toInt());

  done(DL_OK);
}

////////////////////////////////////////////////
void CDlgSrchStarName::on_pushButton_2_clicked()
////////////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////////////////////////////////////
void CDlgSrchStarName::on_listWidget_doubleClicked(const QModelIndex &/*index*/)
////////////////////////////////////////////////////////////////////////////
{
  on_pushButton_clicked();
}
