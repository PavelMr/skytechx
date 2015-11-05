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


////////////////////////////////////////////////////////////////////////////////////////////////
QWidget *CStatusBar::createItem(int id, const QString & tooltip, int width, Qt::Alignment align)
////////////////////////////////////////////////////////////////////////////////////////////////
{
  sbItem_t i;

  i.id = id;
  i.pLabel = new CSBLabel(id);
  connect(i.pLabel, SIGNAL(sigDoubleClicked(int)), this, SLOT(slotDoubleClicked(int)));
  i.pLabel->setAlignment(align | Qt::AlignVCenter);

  if (width > 0)
  {
    i.pLabel->setFixedWidth(width);
  }
  pStatusBar->addWidget(i.pLabel, 0);
  i.pLabel->setToolTip(tooltip);

  tItems.append(i);

  return i.pLabel;
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
  deleteStatusBar();
  restoreStatusBar();
}

void CStatusBar::saveStatusBar()
{
  QSettings set;
  QByteArray data;
  QDataStream ds(&data, QIODevice::WriteOnly);

  ds << (qint32)tItems.count();

  for (int i = 0; i < tItems.count(); i++)
  {
    ds << tItems[i].id;
  }

  set.setValue("status_bar", data);
}

void CStatusBar::restoreStatusBar()
{
  QSettings set;
  QByteArray data;
  QDataStream ds(&data, QIODevice::ReadOnly);

  data = set.value("status_bar").toByteArray();

  if (data.isEmpty())
  {
    addItem(SB_SM_CONST);
    addItem(SB_SM_DATE);
    addItem(SB_SM_TIME);
    addItem(SB_SM_FOV);
    addItem(SB_SM_RA);
    addItem(SB_SM_DEC);
    addItem(SB_SM_AZM);
    addItem(SB_SM_ALT);
    addItem(SB_SM_MAGS);
    addItem(SB_SM_MEASURE);
    return;
  }

  int count;

  ds >> count;
  for (int i = 0; i < count; i++)
  {
    int id;
    ds >> id;

    addItem(id);
  }
}

bool CStatusBar::isUsed(int id)
{
  for (int i = 0; i < tItems.count(); i++)
  {
    if (tItems[i].id == id)
    {
      return true;
    }
  }

  return false;
}

bool CStatusBar::getUsedItem(int index, QString &label, int &id)
{
  if (index < 0 || index >= tItems.count())
  {
    return false;
  }

  label = getLabel(tItems[index].id);
  id = tItems[index].id;

  return true;
}

bool CStatusBar::getAvailableItem(int id, QString &label)
{
  if (!isUsed(id))
  {
    label = getLabel(id);
    return true;
  }

  return false;
}

QString CStatusBar::getLabel(int id)
{
  switch (id)
  {
    case SB_SM_CONST:
      return tr("Constellation under cursor");

    case SB_SM_DATE:
      return tr("Map date");

    case SB_SM_TIME:
      return tr("Map time");

    case SB_SM_FOV:
      return tr("Field of view");

    case SB_SM_RA:
      return tr("Right Ascension");

    case SB_SM_DEC:
      return tr("Declination");

    case SB_SM_AZM:
      return tr("Azimuth");

    case SB_SM_ALT:
      return tr("Altitude");

    case SB_SM_AIRMASS:
      return tr("Airmass");

    case SB_SM_MAGS:
      return tr("Star and DSO magnitude");

    case SB_SM_MEASURE:
      return tr("Measure (Distance/Angle)");

    case SB_SM_MODE:
      return tr("Chart mode");
  }

  return QString();
}

void CStatusBar::slotDoubleClicked(int id)
{
  emit sigDoubleClicked(id);
}

void CStatusBar::addItem(int id)
{
  // TODO: add width to system setting

  QString label = getLabel(id);

  switch (id)
  {
    case SB_SM_CONST:
      createItem(SB_SM_CONST, label, 110, Qt::AlignLeft);
      break;

    case SB_SM_DATE:
      createItem(SB_SM_DATE, label, 100);
      break;

    case SB_SM_TIME:
      createItem(SB_SM_TIME, label, 150);
      break;

    case SB_SM_FOV:
      createItem(SB_SM_FOV, label, 100);
      break;

    case SB_SM_RA:
      createItem(SB_SM_RA, label, 120);
      break;

    case SB_SM_DEC:
      createItem(SB_SM_DEC, label, 120);
      break;

    case SB_SM_AZM:
      createItem(SB_SM_AZM, label, 120);
      break;

    case SB_SM_ALT:
      createItem(SB_SM_ALT, label, 120);
      break;

    case SB_SM_AIRMASS:
      createItem(SB_SM_AIRMASS, label, 80);
      break;

    case SB_SM_MAGS:
      createItem(SB_SM_MAGS, label, 180);
      break;

    case SB_SM_MODE:
      createItem(SB_SM_MODE, label, 50);
      break;

    case SB_SM_MEASURE:
      createItem(SB_SM_MEASURE, label, 180);
      break;
  }
}

//////////////////////////
CSBLabel::CSBLabel(int id)
//////////////////////////
{
  m_id = id;
}

void CSBLabel::mouseDoubleClickEvent(QMouseEvent *)
{
  emit sigDoubleClicked(m_id);
}
