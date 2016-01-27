#ifndef CLINEEDITCOMP_H
#define CLINEEDITCOMP_H

#include <QLineEdit>
#include <QStringListModel>

class CLineEditComp : public QLineEdit
{
  Q_OBJECT
public:
  explicit CLineEditComp(QWidget *parent = 0);
  void setMaxCompleterWords(int nMax = 100);
  void addWord(QString word);
  void addWords(QStringList words);
  void removeWords();

protected:
  QCompleter       *m_completer;
  QStringListModel *m_model;
  int               m_max;
};

#endif // CLINEEDITCOMP_H
