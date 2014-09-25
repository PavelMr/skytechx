#include "ctristatebutton.h"


///////////////////////////////////////////////////
CTriStateButton::CTriStateButton(QWidget *parent) :
  QToolButton(parent)
///////////////////////////////////////////////////
{
  m_state = 0;
  connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTriStateButton::setText(const QString s1, const QPixmap p1, const QString s2, const QPixmap p2, const QString s3, const QPixmap p3)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  str[0] = s1;
  str[1] = s2;
  str[2] = s3;

  pix[0] = p1;
  pix[1] = p2;
  pix[2] = p3;

  QToolButton::setText(str[m_state]);
  QToolButton::setIcon(pix[m_state]);
}


/////////////////////////////////////////
void CTriStateButton::setState(int state)
/////////////////////////////////////////
{
  m_state = state;

  QToolButton::setText(str[m_state]);
  QToolButton::setIcon(pix[m_state]);
}

///////////////////////////////
int CTriStateButton::getState()
///////////////////////////////
{
  return(m_state);
}

///////////////////////////////////
void CTriStateButton::slotClicked()
///////////////////////////////////
{
  if (++m_state >= 3)
    m_state = 0;

  QToolButton::setText(str[m_state]);
  QToolButton::setIcon(pix[m_state]);

  emit sigClicked();
}

