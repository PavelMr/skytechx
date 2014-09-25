#include "clineeditcomp.h"

CLineEditComp::CLineEditComp(QWidget *parent) :
  QLineEdit(parent)
{
  m_back = false;
  setMaxCompleterWords();
  connect(this, SIGNAL(textEdited(QString)), this, SLOT(slotTextEdited(QString)));
}

//////////////////////////////////////////////////
void CLineEditComp::setMaxCompleterWords(int nMax)
//////////////////////////////////////////////////
{
  m_max = nMax;

  while (1)
  {
    if (m_list.count() > m_max)
      m_list.removeFirst();
    else
      break;
  }
}

/////////////////////////////////////////
void CLineEditComp::addWord(QString word)
/////////////////////////////////////////
{
  if (!m_list.contains(word, Qt::CaseInsensitive))
  {
    m_list << word;
  }

  while (1)
  {
    if (m_list.count() > m_max)
      m_list.removeFirst();
    else
      break;
  }
}


void CLineEditComp::keyPressEvent(QKeyEvent *e)
{
  int p = selectionStart();

  m_back = false;

  if (p >= 0)
  {
    switch( e->key() )
    {
      case Qt::Key_Backspace:
      case Qt::Key_Delete:
        m_back = true;
        setText(text().left(p));
        break;

      default:
        QLineEdit::keyPressEvent(e);
    }
  }
  else
  {
    if (e->key() == Qt::Key_Backspace)
      m_back = true;
    QLineEdit::keyPressEvent(e);
  }
}

///////////////////////////////////////////////
void CLineEditComp::slotTextEdited(QString str)
///////////////////////////////////////////////
{
  if (str.isEmpty() || m_back)
  {
    m_back = false;
    return;
  }

  QString it;
  foreach(it, m_list)
  {
    if (it.startsWith(str, Qt::CaseInsensitive))
    {
      int pos = str.count();

      setText(it);
      setSelection(it.count(), -(it.count() - pos));
    }
  }
}
