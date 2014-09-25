#ifndef CTRACKLIST_H
#define CTRACKLIST_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
class CTrackList;
}

class CTrackList : public QDialog
{
  Q_OBJECT
  
public:
  explicit CTrackList(QWidget *parent, double tz);
  ~CTrackList();
  
protected:
  void changeEvent(QEvent *e);
  void reject() {};
  
private:
  Ui::CTrackList *ui;


 private slots:
   void slotDeleteItem(void);
   void on_pushButton_clicked();
   void on_pushButton_3_clicked();
};

#endif // CTRACKLIST_H
