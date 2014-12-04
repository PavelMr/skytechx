#ifndef CADDCUSTOMOBJECT_H
#define CADDCUSTOMOBJECT_H

#include <QDialog>

typedef struct
{
  QString catalogue;
  QStringList list;
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

private slots:
  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_pushButton_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

  void slotDelete();

private:

  void enableItems();
  static void load(QList<customCatalogue_t> *data);
  void save();
  void appendTo(const QString &catalogue, const QString &name);

  Ui::CAddCustomObject *ui;
  QList <customCatalogue_t> m_catalogue;
};

#endif // CADDCUSTOMOBJECT_H
