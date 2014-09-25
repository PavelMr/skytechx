#include "cdsocomnamesearch.h"
#include "ui_cdsocomnamesearch.h"
#include "cdso.h"

CDSOComNameSearch::CDSOComNameSearch(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDSOComNameSearch)
{
    ui->setupUi(this);
    //setFixedSize(width(), height());

    for (int i = 0; i < cDSO.tDsoCommonNames.count(); i++)
    {
      QListWidgetItem *witem = new QListWidgetItem(cDSO.tDsoCommonNames[i].commonName + QString(" (") + cDSO.tDsoCommonNames[i].catName + ")", NULL, i);
      ui->listWidget->addItem(witem);
    }
}

CDSOComNameSearch::~CDSOComNameSearch()
{
    delete ui;
}


void CDSOComNameSearch::changeEvent(QEvent *e)
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

// ok
void CDSOComNameSearch::on_pushButton_clicked()
{
  QList <QListWidgetItem *> item = ui->listWidget->selectedItems();

  if (item.count() == 0)
    return;

   dso_t *dso;
  if (cDSO.findDSO(cDSO.tDsoCommonNames[item[0]->type()].catName, &dso) != -1)
  {
    m_ra = dso->rd.Ra;
    m_dec = dso->rd.Dec;
    m_fov = getOptObjFov(dso->sx / 3600., dso->sy / 3600.);
    done(1);
    return;
  }

  msgBoxError(this, tr("Object not found!"));
  done(0);
}

// cancel
void CDSOComNameSearch::on_pushButton_2_clicked()
{
  done(0);
}

void CDSOComNameSearch::on_listWidget_doubleClicked(const QModelIndex &)
{
  on_pushButton_clicked();
}
