#include "ctipofday.h"
#include "ui_ctipofday.h"
#include "skcore.h"

#include <QDateTime>
#include <QSettings>

#define TIPS_COUNT    12

CTipOfDay::CTipOfDay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTipOfDay)
{
  srand(QDateTime::currentMSecsSinceEpoch());
  m_index = rand() % TIPS_COUNT;

  ui->setupUi(this);
  setFixedSize(size());
  ui->label->setContentsMargins(0, 40, 0, 0);
  setTip(m_index);

  QSettings set;

  bool show = set.value("tip_of_day", true).toBool();

  if (show)
  {
    ui->checkBox->setChecked(true);
  }

  m_show = show;
}

CTipOfDay::~CTipOfDay()
{
  QSettings set;

  set.setValue("tip_of_day", ui->checkBox->isChecked());

  delete ui;
}

void CTipOfDay::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CTipOfDay::setTip(int &index)
{
  if (index >= TIPS_COUNT)
  {
    index = 0;
  }
  else
  if (index < 0)
  {
    index = TIPS_COUNT - 1;
  }

  QString tip[TIPS_COUNT] = {tr("During object search on toolbar press Enter + Shift to locate the object with current field of view."),
                             tr("Keyboard shortcut list and map controls help can be displayed by pressing F2 key."),
                             tr("Double click item on status bar to open particular dialog."),
                             tr("Hold Ctrl key and move mouse cursor over an object to see its name and magnitude."),
                             tr("By clicking left mouse button on an object will display object's information on status bar."),
                             tr("Object's dialog allows you to keep personal notes or to add images to gallery."),
                             tr("To change star appearance go to settings tab Map>Stars>Star bitmap."),
                             tr("Map can be printed in black and white or in any other color profile."),
                             tr("To search for example nearest Moon eclipse in given time range go to menu Observating > Search events"),
                             tr("To get more information about current object press button on object's dialog to go to web server NED or SIMBAD."),
                             tr("Unless stated, all displayed time informations are in given time zone as set in observing location."),
                             tr("Distance or position angle from measure point are displayed on status bar. Measure point will be placed on current mouse cursor by pressing space button."),
                            };

   ui->textBrowser->setText("<br><b><font size=\"5\"> &nbsp;" + tr("Did you know...") + "</font></b><br><hr><br>" + "<font size=\"4\">" + tip[index] + "</font>");
}

void CTipOfDay::on_pushButton_2_clicked()
{
  m_index++;
  setTip(m_index);
}


void CTipOfDay::on_pushButton_3_clicked()
{
  m_index--;
  setTip(m_index);
}
