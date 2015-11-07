#ifndef CSELTELEFIELD_H
#define CSELTELEFIELD_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"

typedef struct
{
  QString name;
  double  par1;
  double  par2;
} teleParam_t;

namespace Ui {
class CSelTeleField;
}

class CSelTeleField : public QDialog
{
  Q_OBJECT

public:
  explicit CSelTeleField(QWidget *parent = 0);
  ~CSelTeleField();

  double  m_outBP;
  double  m_outFOV;
  double  m_outLM;
  double  m_outRP;
  double  m_outMag;
  double  m_outFR;
  double  m_outEP;
  QString m_TEname;
  QString m_name;

protected:
  void changeEvent(QEvent *e);
  void loadTeleItem(QString file, QList <teleParam_t> *list);
  void saveTeleItem(QString file, QListWidget *w);
  void calcParam(void);

private slots:
  void on_pushButton_6_clicked();

  void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  void on_listWidget_2_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  void on_doubleSpinBox_valueChanged(const QString &arg1);

  void on_pushButton_5_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_8_clicked();

private:
  Ui::CSelTeleField *ui;

public slots:
  void deleteItemA();
  void deleteItemB();
};

#endif // CSELTELEFIELD_H
