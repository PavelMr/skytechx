#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include <QDialog>

#include <QItemSelection>
#include <QKeySequence>

class QMenuBar;
class QMenu;
class QStandardItem;

namespace Ui {
class ShortcutDialog;
}

void initShortcutMap(QMenuBar *menuBar);
void saveShortcutMap(QMenuBar *menuBar);

class ShortcutDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ShortcutDialog(QWidget *parent, QMenuBar *menuBar);
  ~ShortcutDialog();

private slots:
  void selectionChanged(const QItemSelection &, const QItemSelection &);

  void on_toolButton_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_5_clicked();

private:
  Ui::ShortcutDialog *ui;
  void enumerateMenu(QMenu *menu, int level);
  QStandardItem *m_currentItem;
  QMap <QAction *, QKeySequence> m_map;
  void setUserShortcuts();
  bool checkDuplicates(QKeySequence &key, QAction **action);
};

#endif // SHORTCUTDIALOG_H
