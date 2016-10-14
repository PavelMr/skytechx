#include "clineeditcomp.h"

#include <QCompleter>

CLineEditComp::CLineEditComp(QWidget *parent) :
  QLineEdit(parent)
{
  m_completer = new QCompleter();
  m_model = new QStringListModel();

  m_completer->setModel(m_model);
  m_completer->setMaxVisibleItems(16);
  m_completer->setCaseSensitivity(Qt::CaseInsensitive);
  m_completer->setCompletionMode(QCompleter::PopupCompletion);
  m_completer->setFilterMode(Qt::MatchContains );

  setCompleter(m_completer);

  m_max = 1000;
}

//////////////////////////////////////////////////
void CLineEditComp::setMaxCompleterWords(int nMax)
//////////////////////////////////////////////////
{
  m_max = nMax;
}

/////////////////////////////////////////
void CLineEditComp::addWord(QString word)
/////////////////////////////////////////
{
  QStringList list = m_model->stringList(); 

  if (!list.contains(word, Qt::CaseInsensitive))
  {
    list << word;
  }
  else
  {
    return;
  }

  while (1)
  {
    if (list.count() > m_max)
      list.removeFirst();
    else
      break;
  }

  m_model->setStringList(list);
}

void CLineEditComp::addWords(QStringList words)
{   
  foreach (QString word, words)
  {
    addWord(word);
  }
}

void CLineEditComp::addWordsAlways(QStringList words)
{
  QStringList list = m_model->stringList();
  m_model->setStringList(list + words);
}

void CLineEditComp::removeWords()
{
  m_model->setStringList(QStringList());
}

