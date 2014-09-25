#ifndef CLINEEDITCOMP_H
#define CLINEEDITCOMP_H

#include <QtGui>
#include <QtWidgets>

class CLineEditComp : public QLineEdit
{
  Q_OBJECT
public:
  explicit CLineEditComp(QWidget *parent = 0);
  void setMaxCompleterWords(int nMax = 100);
  void addWord(QString word);

protected:
  void keyPressEvent(QKeyEvent *e);

  int         m_max;
  QStringList m_list;
  bool        m_back;
  
signals:
  
public slots:
  void slotTextEdited(QString str);
  
};

#endif // CLINEEDITCOMP_H
