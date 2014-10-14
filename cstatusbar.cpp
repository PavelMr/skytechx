#include "cstatusbar.h"


////////////////////////////////////////
CStatusBar::CStatusBar(QStatusBar *pBar)
////////////////////////////////////////
{
  pStatusBar = pBar;
}

//////////////////////////////////////
void CStatusBar::deleteStatusBar(void)
//////////////////////////////////////
{
  for (int i = 0; i < tItems.count(); i++)
  {
    delete tItems[i].pLabel;
  }
  tItems.clear();
}


////////////////////////////////////////////////////////////////////////////////////////////
void CStatusBar::createItem(int id,  const QString & tooltip, int width, Qt::Alignment align)
////////////////////////////////////////////////////////////////////////////////////////////
{
  sbItem_t i;

  i.id = id;
  i.pLabel = new QLabel();
  i.pLabel->setAlignment(align | Qt::AlignVCenter);
  if (id == SB_SEP)
  {
    i.pLabel->setText("   ");
  }    

  if (width > 0)
  {
    i.pLabel->setFixedWidth(width);
  }
  pStatusBar->addWidget(i.pLabel, 0);
  i.pLabel->setToolTip(tooltip);

  tItems.append(i);
}


/////////////////////////////////////////////
void CStatusBar::setItem(int id, QString str)
/////////////////////////////////////////////
{
  for (int i = 0; i < tItems.count(); i++)
  {
    if (tItems[i].id == id)
    {
      tItems[i].pLabel->setText("  " + str + "  ");
      break;
    }
  }
}

//////////////////////////////////////
void CStatusBar::createSkyMapBar(void)
//////////////////////////////////////
{
  createItem(SB_SM_CONST, tr("Constellation under cursor"), 110, Qt::AlignLeft);
  createItem(SB_SM_DATE, tr("Map date"), 100);
  createItem(SB_SM_TIME, tr("Map time"), 150);
  createItem(SB_SM_FOV, tr("Field of view"), 100);
  createItem(SB_SM_RA, tr("Right Ascension"), 120);
  createItem(SB_SM_DEC, tr("Declination"), 110);
  createItem(SB_SM_AZM, tr("Azimuth"), 120);
  createItem(SB_SM_ALT, tr("Altitude"), 120);
  createItem(SB_SM_MAGS, tr("Star and DSO magnitude"), 170);
  createItem(SB_SM_MEASURE, tr("Measure (Distance/Angle)"), 160);

  //QLineEdit *edit = new QLineEdit;
  //pStatusBar->addPermanentWidget(edit, 200);
}
