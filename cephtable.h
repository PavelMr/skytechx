#ifndef CEPHTABLE_H
#define CEPHTABLE_H

#include <QtGui>
#include <QtWidgets>

typedef struct
{
  QStringList row;
} tableRow_t;

namespace Ui {
  class CEphTable;
}

class CEphTable : public QDialog
{
  Q_OBJECT
  
public:
  explicit CEphTable(QWidget *parent, QString name, QStringList header, QList <tableRow_t> row);
  ~CEphTable();

protected:
  QString m_name;
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

private:
  Ui::CEphTable *ui;
};

#endif // CEPHTABLE_H
