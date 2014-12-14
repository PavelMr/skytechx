#ifndef CADDCUSTOMOBJECT_H
#define CADDCUSTOMOBJECT_H

#include <QDialog>

typedef struct
{
  QString catalogue;
  QList <int> list;
} customCatalogue_t;

namespace Ui {
class CAddCustomObject;
}

class CAddCustomObject : public QDialog
{
  Q_OBJECT

public:
  explicit CAddCustomObject(QWidget *parent, const QString &name);
  ~CAddCustomObject();
  static void load(QList<customCatalogue_t> *data);

private slots:
  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_pushButton_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

  void slotDelete();

private:

  void enableItems();
  void save();
  bool appendTo(const QString &catalogue, const QString &name);

  Ui::CAddCustomObject *ui;
  QList <customCatalogue_t> m_catalogue;
};

#endif // CADDCUSTOMOBJECT_H
