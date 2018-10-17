#include "shortcutdialog.h"
#include "ui_shortcutdialog.h"
#include "skutils.h"

#include <QStandardItemModel>
#include <QMenuBar>
#include <QMenu>
#include <QLineEdit>

#include <QDebug>

static QMap <QAction *, QKeySequence> m_defaultMap;
static QList <QKeySequence> m_mapKeys;

static void enumerateMenu(QMenu *menu)
{
  QSettings set;

  foreach (QAction *action, menu->actions())
  {
    if (action->isSeparator())
    {
      continue;
    } else if (action->menu())
    {
      enumerateMenu(action->menu());
    } else
    {
      if (m_defaultMap.contains(action))
      {
        qDebug() << "ERROR enumerateMenu (duplicate)" << action->text();
      }
      m_defaultMap[action] = action->shortcut();
      action->setShortcut(set.value("shortcuts/" + action->text().replace("~", "/")).value<QKeySequence>());
    }
  }
}

void initShortcutMap(QMenuBar *menuBar)
{
  foreach (QAction *a, menuBar->actions())
  {
    enumerateMenu(a->menu());
  }

  // NOTE: add new map keys here !!!!!!!!!
  m_mapKeys.append(QKeySequence("Up"));
  m_mapKeys.append(QKeySequence("Down"));
  m_mapKeys.append(QKeySequence("Right"));
  m_mapKeys.append(QKeySequence("Left"));

  m_mapKeys.append(QKeySequence("I"));
  m_mapKeys.append(QKeySequence("T"));
  m_mapKeys.append(QKeySequence("L"));
  m_mapKeys.append(QKeySequence("F"));
  m_mapKeys.append(QKeySequence("Z"));

  m_mapKeys.append(QKeySequence("+"));
  m_mapKeys.append(QKeySequence("-"));

  m_mapKeys.append(QKeySequence("Space"));

  m_mapKeys.append(QKeySequence("Ins"));
  m_mapKeys.append(QKeySequence("Home"));
  m_mapKeys.append(QKeySequence("PgUp"));

  m_mapKeys.append(QKeySequence("Enter"));

  m_mapKeys.append(QKeySequence("Backspace"));
  m_mapKeys.append(QKeySequence("Esc"));
  m_mapKeys.append(QKeySequence("Del"));
}

static void enumerateAndSave(QMenu *menu, QSettings &set)
{
  foreach (QAction *action, menu->actions())
  {
    if (action->isSeparator())
    {
      continue;
    } else if (action->menu())
    {
      enumerateAndSave(action->menu(), set);
    } else
    {
      m_defaultMap[action] = action->shortcut();
      set.setValue("shortcuts/" + action->text().replace("/", "~"), action->shortcut());
    }
  }
}

void saveShortcutMap(QMenuBar *menuBar)
{
  QSettings set;

  foreach (QAction *a, menuBar->actions())
  {
    enumerateAndSave(a->menu(), set);
  }
}

ShortcutDialog::ShortcutDialog(QWidget *parent, QMenuBar *menuBar) :
  QDialog(parent),
  ui(new Ui::ShortcutDialog)
{
  ui->setupUi(this);  

  QStandardItemModel *model = new QStandardItemModel(0, 3);
  model->setHeaderData(0, Qt::Horizontal, tr("Item"));
  model->setHeaderData(1, Qt::Horizontal, tr("Description"));
  model->setHeaderData(2, Qt::Horizontal, tr("Shortcut"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);

  foreach (QAction *a, menuBar->actions())
  {
    QStandardItem *item = new QStandardItem(a->text().remove("&"));
    QFont font(item->font());

    font.setBold(true);
    item->setFont(font);
    item->setEditable(false);
    item->setData(QVariant(0));

    model->appendRow(item);

    enumerateMenu(a->menu(), 0);
  }

  ui->treeView->header()->resizeSection(0, 250);
  ui->treeView->header()->resizeSection(1, 250);

  connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
          this, SLOT(selectionChanged(const QItemSelection&,const QItemSelection&)));

  setUserShortcuts();
}

ShortcutDialog::~ShortcutDialog()
{
  delete ui;
}


void ShortcutDialog::enumerateMenu(QMenu *menu, int level)
{
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  foreach (QAction *action, menu->actions())
  {
    if (action->isSeparator())
    {
        continue;
    } else if (action->menu())
    {
      QStandardItem *item = new QStandardItem("    "  + action->text().remove("&"));
      QFont font(item->font());

      font.setBold(true);
      item->setFont(font);
      item->setEditable(false);
      item->setData(QVariant(0));

      model->appendRow(item);
      enumerateMenu(action->menu(), level + 1);
    } else
    {
      QStandardItem *item0 = new QStandardItem(QString("   ").rightJustified(level * 5, ' ') + action->text().remove("&"));
      QStandardItem *item1 = new QStandardItem(action->toolTip());
      QStandardItem *item2 = new QStandardItem(action->shortcut().toString());
      QList <QStandardItem *> list;

      item0->setData(QVariant::fromValue(action));
      item0->setEditable(false);
      item1->setEditable(false);
      item2->setEditable(false);
      item2->setData(QVariant::fromValue(action));

      m_map[action] = action->shortcut();

      list << item0 << item1 << item2;

      model->appendRow(list);
    }
  }
}

void ShortcutDialog::setUserShortcuts()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 2);

    if (item == nullptr) continue;

    QAction *action = item->data().value<QAction *>();

    QFont font(item->font());
    font.setBold(m_map[action] != m_defaultMap[action]);
    item->setFont(font);
  }
}


void ShortcutDialog::selectionChanged(const QItemSelection & ,const QItemSelection &)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();

  if (il.count() == 0)
  {
    m_currentItem = nullptr;
    return;
  }

  QStandardItem *item = model->itemFromIndex(il.at(0));
  QAction *action = item->data().value<QAction *>();

  if (action == nullptr)
  {
    ui->keySequenceEdit->clear();
    ui->keySequenceEdit->setEnabled(false);
    ui->treeView->setFocus();
    return;
  }

  m_currentItem = model->itemFromIndex(il.at(2));

  ui->keySequenceEdit->setEnabled(true);
  if (action->shortcut().isEmpty())
  {
    ui->keySequenceEdit->clear();
  }
  else
  {
    ui->keySequenceEdit->setKeySequence(action->shortcut());
  }
  ui->treeView->setFocus();
}


void ShortcutDialog::on_toolButton_clicked()
{
  if (ui->keySequenceEdit->isEnabled())
  {
    ui->keySequenceEdit->clear();
  }
}

bool ShortcutDialog::checkDuplicates(QKeySequence &key, QAction **action)
{
  if (m_map.keys(key).count() > 0)
  {
    *action = m_map.key(key);
    return true;
  }

  // TODO: check map keys
  if (m_mapKeys.contains(key))
  {
    *action = nullptr;
    return true;
  }

  return false;
}

// on assign
void ShortcutDialog::on_pushButton_4_clicked()
{
  if (m_currentItem && ui->keySequenceEdit->keySequence().count() > 0)
  {
    QAction *tmp;
    QKeySequence key = ui->keySequenceEdit->keySequence()[0];
    if (checkDuplicates(key, &tmp))
    {
      QString text;

      if (tmp)
      {
        text = tmp->text();
      }
      else
      {
        text = tr("Used in map control");
      }

      msgBoxError(this, tr("Already used!!!\n") + text);
      ui->keySequenceEdit->clear();
      return;
    }

    m_currentItem->setText(key.toString());
    QAction *action = m_currentItem->data().value<QAction *>();
    m_map[action] = ui->keySequenceEdit->keySequence();
    setUserShortcuts();
  }
  else if (m_currentItem)
  {
    m_currentItem->setText("");
    QAction *action = m_currentItem->data().value<QAction *>();
    m_map[action] = QKeySequence();
    setUserShortcuts();
  }


}

// on OK
void ShortcutDialog::on_pushButton_clicked()
{
  QMap<QAction *, QKeySequence>::iterator i;

  for (i = m_map.begin(); i != m_map.end(); ++i)
  {
    i.key()->setShortcut(i.value());
  }

  done(QDialog::Accepted);
}

void ShortcutDialog::on_pushButton_2_clicked()
{
  done(QDialog::Rejected);
}

void ShortcutDialog::on_pushButton_3_clicked()
{
  if (m_currentItem)
  {
    QAction *action = m_currentItem->data().value<QAction *>();
    m_currentItem->setText(m_defaultMap[action].toString());
    ui->keySequenceEdit->setKeySequence(m_defaultMap[action]);
    m_map[action] = ui->keySequenceEdit->keySequence();
    setUserShortcuts();
  }
}

void ShortcutDialog::on_pushButton_5_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 2);

    if (item == nullptr) continue;

    QAction *action = item->data().value<QAction *>();
    item->setText(m_defaultMap[action].toString());
    m_map[action] = m_defaultMap[action];
  }
  setUserShortcuts();
}
