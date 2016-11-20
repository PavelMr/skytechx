#include "cadvsearch.h"
#include "ui_cadvsearch.h"
#include "clunarfeatures.h"
#include "ccomdlg.h"
#include "casterdlg.h"
#include "csgp4.h"
#include "cmeteorshower.h"
#include "gcvs.h"

#include "csearch.h"

static int lastRadio = 0;

CAdvSearch::CAdvSearch(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CAdvSearch)
{
  ui->setupUi(this);
  m_mapView = *view;

  ui->lineEdit->setMaxCompleterWords(100000);

  switch (lastRadio)
  {
    case 0:
      ui->radioButton_all->setChecked(true);
      break;
    case 1:
      ui->radioButton->setChecked(true);
      break;
    case 2:
      ui->radioButton_2->setChecked(true);
      break;
    case 3:
      ui->radioButton_3->setChecked(true);
      break;
    case 4:
      ui->radioButton_4->setChecked(true);
      break;
    case 5:
      ui->radioButton_5->setChecked(true);
      break;
    case 6:
      ui->radioButton_6->setChecked(true);
      break;
    case 7:
      ui->radioButton_7->setChecked(true);
      break;
    case 8:
      ui->radioButton_8->setChecked(true);
      break;
    case 9:
      ui->radioButton_9->setChecked(true);
      break;
    case 10:
      ui->radioButton_10->setChecked(true);
      break;
    case 11:
      ui->radioButton_11->setChecked(true);
      break;
  }

  connect(ui->radioButton_all , SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_2, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_3, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_4, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_5, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_6, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_7, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_8, SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_9 , SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_10 , SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));
  connect(ui->radioButton_11 , SIGNAL(toggled(bool)), this, SLOT(slotRadioChange()));

  slotRadioChange();
}

CAdvSearch::~CAdvSearch()
{
  delete ui;
}

void CAdvSearch::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CAdvSearch::on_pushButton_2_clicked()
{
  QString prefix;

  if (ui->radioButton_all->isChecked())
    prefix = SS_ALL;
  else if (ui->radioButton->isChecked())
    prefix = SS_PLANET;
  else if (ui->radioButton_2->isChecked())
    prefix = SS_STAR;
  else if (ui->radioButton_3->isChecked())
    prefix = SS_STAR_NAME;
  else if (ui->radioButton_4->isChecked())
    prefix = SS_CONSTEL;
  else if (ui->radioButton_5->isChecked())
    prefix = SS_DSO;
  else if (ui->radioButton_6->isChecked())
    prefix = SS_ART_SAT;
  else if (ui->radioButton_7->isChecked())
    prefix = SS_ASTER;
  else if (ui->radioButton_8->isChecked())
    prefix = SS_COMET;
  else if (ui->radioButton_9->isChecked())
    prefix = SS_LUNAR_FEAT;
  else if (ui->radioButton_10->isChecked())
    prefix = SS_SHOWER;
  else if (ui->radioButton_11->isChecked())
    prefix = SS_VOT;

  QString text;

  text = prefix + ui->lineEdit->text();

  if (!CSearch::search(&m_mapView, text, m_ra, m_dec, m_fov, m_mapObj))
  {
    QToolTip::showText(ui->lineEdit->mapToGlobal( QPoint( 0, 0 ) ), tr("Not found!!!"));
    return;
  }

  done(DL_OK);
}

void CAdvSearch::on_lineEdit_textChanged(const QString &)
{
  QToolTip::hideText();
}

void CAdvSearch::slotRadioChange()
{
  QStringList list;

  ui->lineEdit->removeWords();  

  bool isAll = ui->radioButton_all->isChecked();

  if (ui->radioButton->isChecked() || isAll)
  {
    lastRadio = 1;
    CAstro astro;
    for (int i = PT_SUN; i <= PT_MOON; i++)
    {      
      list << astro.getName(i);
      list << astro.getFileName(i);
    }
  }

  if (ui->radioButton_2->isChecked() || isAll)
  {
    lastRadio = 2;
    list << "TYC";
    list << "UCAC4";
    list << "USNO2";
    list << "GSC";
    list << "HD";

    list << (g_GCVS.nameList());
  }

  if (ui->radioButton_3->isChecked() || isAll)
  {
    lastRadio = 3;
    for (int i = 0; i < cTYC.tNames.count(); i++)
    {
      int offs = cTYC.tNames[i]->supIndex;
      QString name = cTYC.getStarName(&cTYC.pSupplement[offs]);
      list << name;
    }
  }

  if (ui->radioButton_4->isChecked() || isAll)
  {
    lastRadio = 4;
    list << constGetNameList();
  }

  if (ui->radioButton_5->isChecked() || isAll)
  {
    lastRadio = 5;
    list << cDSO.getCommonNameList();
  }

  if (ui->radioButton_6->isChecked() || isAll)
  {
    lastRadio = 6;
    for (int i = 0; i < sgp4.count(); i++)
    {
      if (sgp4.tleItem(i)->used)
      {
        list << sgp4.getName(i);
      }
    }
  }

  if (ui->radioButton_7->isChecked() || isAll)
  {
    lastRadio = 7;
    for (int i = 0; i < tAsteroids.count(); i++)
    {
      if (tAsteroids[i].selected)
      {
        list << tAsteroids[i].name;
      }
    }
  }

  if (ui->radioButton_8->isChecked() || isAll)
  {
    lastRadio = 8;
    for (int i = 0; i < tComets.count(); i++)
    {
      if (tComets[i].selected)
      {
        list << tComets[i].name;
      }
    }
  }

  if (ui->radioButton_9->isChecked() || isAll)
  {
    lastRadio = 9;
    list << cLunarFeatures.getNames();
  }

  if (ui->radioButton_10->isChecked() || isAll)
  {
    lastRadio = 10;
    list << g_meteorShower.getNameList();
  }

  if (ui->radioButton_11->isChecked() || isAll)
  {
    lastRadio = 11;
  }

  if (ui->radioButton_all->isChecked())
  {
    lastRadio = 0;
  }    

  list.removeDuplicates();

  ui->lineEdit->addWordsAlways(list);
}
