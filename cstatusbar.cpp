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


///////////////////////////////////////////////////////////////////
void CStatusBar::createItem(int id, int width, Qt::Alignment align)
///////////////////////////////////////////////////////////////////
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
  createItem(SB_SM_CONST, 110, Qt::AlignLeft);
  createItem(SB_SM_DATE, 100);
  createItem(SB_SM_TIME, 150);
  createItem(SB_SM_FOV, 100);
  createItem(SB_SM_RA, 120);
  createItem(SB_SM_DEC, 110);
  createItem(SB_SM_AZM, 120);
  createItem(SB_SM_ALT, 120);
  createItem(SB_SM_MAGS, 170);
  createItem(SB_SM_MEASURE, 160);

  //QLineEdit *edit = new QLineEdit;
  //pStatusBar->addPermanentWidget(edit, 200);
}
