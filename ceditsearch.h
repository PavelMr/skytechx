#ifndef CEDITSEARCH_H
#define CEDITSEARCH_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class CEditSearch : public QWidget
{
  Q_OBJECT
public:
  explicit CEditSearch(QWidget *parent = 0);

signals:
  void sigSearchChanged(const QString &text);
  void sigSearchNext(const QString &text);

public slots:
  void slotTextChanged(const QString & text);
  void slotNextClicked();

private:
  QLineEdit *m_edit;
  QPushButton *m_pbNext;
};

#endif // CEDITSEARCH_H
