#ifndef CTIMELAPSEPREF_H
#define CTIMELAPSEPREF_H

#include <QDialog>

#define DEFAULT_TIME_LAPSE_UPDATE      100

namespace Ui {
class CTimeLapsePref;
}

class CTimeLapsePref : public QDialog
{
  Q_OBJECT

public:
  explicit CTimeLapsePref(QWidget *parent = 0);
  ~CTimeLapsePref();
  int m_multiplicator;
  int m_updateSpeed;

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::CTimeLapsePref *ui;
};

#endif // CTIMELAPSEPREF_H
