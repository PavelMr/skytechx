#include "ctipofday.h"
#include "ui_ctipofday.h"
#include "skcore.h"

#include <QDateTime>
#include <QSettings>

#define TIPS_COUNT    9

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

  // TODO: prelozit a pak pak i v linguistu

  QString tip[TIPS_COUNT] = {"Při hledání objektu v nástrojové liště pokud stiskněte Enter spolu s klávesou Shift tak se objekt vycentruje ale zorné pole se nezmění.",
                             // "(nástrojové liště) toolbar (zorné pole) field of view",

                             "Nápovědu pro klávesnicové zkratky a ovládání mapy zobrazíte klávesou F2.",
                             //  (klávesnicové zkratky) keyboard shortcut

                             "Kliknutím levého tlačítka myši na object se zobrazí v postraním panelu informace o objektu.",
                             // (postraním panelu) side bar

                             "V dialogu objektu lze přidat vlastní poznámky nebo přiřadit do galerie obrázky.",

                             "Pokud chcete změnit vzhled hvězd tak ho můžete změnit v nastavení pod záložkou Mapa->Hvězdy->Bitmapa hvězdy.",

                             "Tisk mapy lze tisknout jak černobíle tak v jakémkoli jiném barevném profilu.",

                             "Pokud chcete vyhledat např. nejbližší zatmění Měsíce ve Vámi nastavené poloze tak ho můžete najít v menu 'Pozorování->Hledání událostí...'",

                             "Pokud chcete o objektu získat více informací tak v dialogu objektu lze stiskem tlačítka přejít na webový server NED nebo SIMBAD kde se zobrazí podrobnější informace.",

                             "Vzdálenost a poziční ůhel od pozice kurzoru na mapě a měřícího bodu se zobrazuje ve stavovém řádku. Přesun měřícího bodu lze provést klávesou Space."
                             // (poziční ůhel) Position angle (měřícího bodu) measure point (stavovém řádku) status bar

                            };

 // ui->textBrowser->setText("<br><h2>" + tr("Did you know...") + "</h2><hr><br>" + "<font size=\"4\">" + tip[index] + "</font>");
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
