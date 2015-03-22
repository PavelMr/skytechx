#include "chorizoneditor.h"
#include "ui_chorizoneditor.h"

#include <QFileDialog>

CHorizonEditor::CHorizonEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CHorizonEditor)
{
  ui->setupUi(this);
  ui->frame->setEditor(ui->plainTextEdit);
  ui->frame->setCurrentText(ui->lineEdit->text());
}

CHorizonEditor::~CHorizonEditor()
{
  delete ui;
}

void CHorizonEditor::on_lineEdit_textChanged(const QString &arg1)
{
  ui->frame->setCurrentText(arg1);
}

void CHorizonEditor::on_pushButton_clicked()
{
  QString name = QFileDialog::getOpenFileName(this, tr("Open a File"),
                              QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/horizons", "Horizon file (*.hrz)");

  if (!name.isEmpty())
  {
    ui->frame->load(name);
  }
}

void CHorizonEditor::on_tabWidget_currentChanged(int index)
{
  if (index == 0)
  {
    QTemporaryFile name(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/horizons" + "/XXXXXX");

    if (name.open())
    {
      QFile f(name.fileName());
      QTextStream ts(&f);

      qDebug() << name.fileName();

      if (f.open(QFile::WriteOnly | QFile::Text))
      {
        ts << ui->plainTextEdit->toPlainText();
        f.close();
      }
      if (!ui->frame->load(name.fileName()))
      {
        ui->tabWidget->setCurrentIndex(1);
        msgBoxError(this, tr("Error in parsing file!"));
      }
      f.remove();
    }
  }
}

void CHorizonEditor::on_pushButton_5_clicked()
{
  ui->frame->removeTexture();
}

void CHorizonEditor::on_pushButton_4_clicked()
{
  QString name = QFileDialog::getOpenFileName(this, tr("Open a File"),
                              QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/horizons", "PNG File (*.png)");

  if (!name.isEmpty())
  {
    ui->frame->setTexture(name);
  }
}

void CHorizonEditor::on_pushButton_2_clicked()
{
  QString name = QFileDialog::getSaveFileName(this, tr("Save a File"),
                              QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/horizons", "Horizon file (*.hrz)");

  if (!name.isEmpty())
  {
    QFile f(name);
    QTextStream ts(&f);

    if (f.open(QFile::WriteOnly | QFile::Text))
    {
      ts << ui->plainTextEdit->toPlainText();
      f.close();
    }
  }
}

void CHorizonEditor::on_pushButton_3_clicked()
{
  done(DL_OK);
}
