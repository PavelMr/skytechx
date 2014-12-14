#include "caddcustomobject.h"
#include "ui_caddcustomobject.h"
#include "ctextsel.h"
#include "skcore.h"
#include "cdso.h"

static QString lastCatalogue;

CAddCustomObject::CAddCustomObject(QWidget *parent, const QString &name) :
  QDialog(parent),
  ui(new Ui::CAddCustomObject)
{
  ui->setupUi(this);
  ui->lineEdit->setText(name);

  QStandardItemModel *model = new QStandardItemModel;

  ui->listView->setModel(model);
  QShortcut *sh = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listView, 0, 0,  Qt::WidgetShortcut);
  connect(sh, SIGNAL(activated()), this, SLOT(slotDelete()));

  setWindowTitle(tr("Custom catalogue"));
  load(&m_catalogue);

  foreach (const customCatalogue_t &item, m_catalogue)
  {
    ui->comboBox->addItem(item.catalogue);
  }

  ui->comboBox->setCurrentText(lastCatalogue);
  enableItems();
}

CAddCustomObject::~CAddCustomObject()
{
  lastCatalogue = ui->comboBox->currentText();
  save();

  delete ui;
}

void CAddCustomObject::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void CAddCustomObject::on_comboBox_currentIndexChanged(const QString &arg1)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  model->removeRows(0, model->rowCount());

  foreach (customCatalogue_t item, m_catalogue)
  {
    if (item.catalogue == arg1)
    {
      int i = 0;
      foreach (const int id, item.list)
      {
        QStandardItem *item = new QStandardItem;
        dso_t *dso = &cDSO.dso[id];

        if (id >= 0 || id < cDSO.dsoHead.numDso)
        {
          item->setText(cDSO.getName(dso, 0));
          item->setEditable(false);
          model->insertRow(i, item);
          i++;
        }
      }
      return;
    }
  }
}

// add new catalogue
void CAddCustomObject::on_pushButton_clicked()
{
  CTextSel dlg(this, tr("Create new catalogue"), 64, "");

  if (dlg.exec() == DL_OK)
  {
    customCatalogue_t cc;

    if (ui->comboBox->findText(dlg.m_text) != -1)
    {
      msgBoxError(this, tr("This catalogue already exists!"));
      return;
    }

    cc.catalogue = dlg.m_text;
    m_catalogue.append(cc);

    ui->comboBox->addItem(cc.catalogue);
    ui->comboBox->setCurrentText(cc.catalogue);
    enableItems();
  }
}

void CAddCustomObject::on_pushButton_5_clicked()
{
  QString name = ui->lineEdit->text();

  if (name.simplified().isEmpty())
  {
    return;
  }

  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  if (appendTo(ui->comboBox->currentText(), name))
  {
    QStandardItem *item = new QStandardItem;
    dso_t *dso;

    int index = cDSO.findDSO((char *)qPrintable(name), &dso);

    item->setText(cDSO.getName(dso, 0));
    item->setEditable(false);
    model->insertRow(model->rowCount(), item);
    ui->lineEdit->setText("");
  }
}

void CAddCustomObject::enableItems()
{
  bool enable;

  if (ui->comboBox->count() == 0)
  {
    enable = false;
  }
  else
  {
    enable = true;
  }

  ui->lineEdit->setEnabled(enable);
  ui->pushButton_5->setEnabled(enable);
  ui->pushButton_4->setEnabled(enable);
}

void CAddCustomObject::load(QList <customCatalogue_t> *data)
{
  SkFile f("data/dso/custom.dat");

  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    customCatalogue_t cc;
    while (!f.atEnd())
    {
      QString line = f.readLine();
      if (line.startsWith("#"))
      {
        if (!cc.catalogue.isEmpty())
        {
          data->append(cc);
        }
        cc.list.clear();
        cc.catalogue = "";
        line = line.remove(0, 1);
        cc.catalogue = line.simplified();
      }
      else
      {
        cc.list.append(line.simplified().toInt());
      }
    }
    if (!cc.catalogue.isEmpty())
    {
      data->append(cc);
    }
  }
}

void CAddCustomObject::save()
{
  SkFile f("data/dso/custom.dat");
  QTextStream s(&f);

  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    foreach (const customCatalogue_t &item, m_catalogue)
    {
      s << "#" << item.catalogue << "\n";
      foreach (const int id, item.list)
      {
        s << id << "\n";
      }
    }
  }
}

bool CAddCustomObject::appendTo(const QString &catalogue, const QString &name)
{
  dso_t *dso;
  int index = cDSO.findDSO((char *)qPrintable(name), &dso);

  if (index == -1)
  {
    // dso not found
    msgBoxError(this, tr("Object not found!"));
    return false;
  }

  for(QList <customCatalogue_t>::iterator it = m_catalogue.begin(); it != m_catalogue.end(); ++it)
  {
    customCatalogue_t &cat = *it;

    if (cat.catalogue == catalogue)
    {
      if (cat.list.contains(index))
      {
        msgBoxError(this, tr("Object is already in the list!"));
        return false;
      }
      cat.list.append(index);
      return true;
    }
  }

  qDebug() << "append error";

  return false;
}

// remove
void CAddCustomObject::on_pushButton_4_clicked()
{
  if (msgBoxQuest(this, tr("Do you wish remove this catalogue?")) == QMessageBox::Yes)
  {
    int index = ui->comboBox->currentIndex();
    ui->comboBox->removeItem(index);
    m_catalogue.removeAt(index);

    enableItems();
  }
}

void CAddCustomObject::slotDelete()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  model->removeRow(index);
  m_catalogue[ui->comboBox->currentIndex()].list.removeAt(index);
}

