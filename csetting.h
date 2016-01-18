#ifndef CSETTING_H
#define CSETTING_H

#include <QDialog>
#include "cfontcolorbutton.h"
#include "setting.h"
#include "curlfile.h"

namespace Ui {
  class CSetting;
}

class CSetting : public QDialog
{
  Q_OBJECT

public:
  explicit CSetting(QWidget *parent = 0);
  ~CSetting();

protected:
   setting_t  set;

   void setValues(void);
   void apply(void);
   void setFontColor(int fnt, CFontColorButton *bt);
   void paintEvent(QPaintEvent *);
   void setProfileLabel(void);
   void fillProfiles(void);
   bool resetQuestion();
   void fillGamepad();
   void applyGamepad();
   void fillConstNames();

public slots:
   void onTreeWidgetCustomContextMenuRequested(const QPoint &pos);

private slots:
   void on_pushButton_2_clicked();

   void on_pushButton_clicked();

   void on_pushButton_4_clicked();

   void on_pushButton_5_clicked();

   void on_pushButton_6_clicked();

   void slotStarMagChange(int);
   void slotDSOMagChange(int);

   void on_treeView_2_clicked(const QModelIndex &index);

   void on_pushButton_7_clicked();

   void on_pushButton_8_clicked();

   void on_pushButton_9_clicked();

   void on_pushButton_10_clicked();

   void on_pushButton_11_clicked();

   void on_pushButton_12_clicked();

   void on_pushButton_13_clicked();

   void on_pushButton_14_clicked();

   void on_pushButton_15_clicked();

   void on_pushButton_16_clicked();

   void on_pushButton_18_clicked();

   void on_pushButton_19_clicked();

   void on_pushButton_20_clicked();

   void on_pushButton_21_clicked();

   void on_pushButton_22_clicked();

   void on_pushButton_23_clicked();

   void on_pushButton_24_clicked();

   void on_pushButton_25_clicked();

   void on_pushButton_26_clicked();

   void on_pushButton_27_clicked();

   void on_pushButton_28_clicked();

   void on_pushButton_30_clicked();

   void on_pushButton_29_clicked();

   void on_listWidget_2_itemSelectionChanged();

   void on_pushButton_34_clicked();

   void on_pushButton_33_clicked();

   void on_pushButton_35_clicked();

   void on_pushButton_36_clicked();

   void on_pushButton_37_clicked();

   void on_pushButton_38_clicked();

   void on_pushButton_39_clicked();

   void on_pushButton_40_clicked();

   void on_pushButton_41_clicked();

   void on_pushButton_42_clicked();

   void on_pushButton_43_clicked();

   void on_pushButton_44_clicked();

   void on_pushButton_45_clicked();

   void on_pushButton_32_clicked();

   void on_pushButton_47_clicked();

   void on_pushButton_48_clicked();

   void on_pushButton_49_clicked();

   void on_pushButton_46_clicked();

   void on_pushButton_3_clicked();

   void on_pushButton_50_clicked();

   void on_pushButton_17_clicked();

   void on_pushButton_52_clicked();

   void on_pushButton_51_clicked();

   void on_pushButton_53_clicked();

   void on_pushButton_54_clicked();

   void on_pushButton_55_clicked();

   void on_pushButton_56_clicked();

   void on_pushButton_57_clicked();

   void on_pushButton_58_clicked();

   void on_pushButton_31_clicked();

   void on_listView_doubleClicked(const QModelIndex &index);

   void on_pushButton_59_clicked();

   void on_cb_device_currentIndexChanged(int index);

   void on_pushButton_60_clicked();

   void on_pushButton_61_clicked();

   void on_toolButton_clicked();

   void on_pushButton_63_clicked();

   void on_pushButton_62_clicked();

   void on_pushButton_64_clicked();

   void on_pushButton_65_clicked();

   void on_pushButton_66_clicked();

   void on_pushButton_urat_browse_clicked();

   void on_toolButton_2_clicked();

   void starBitmapChange();

   void on_tabWidget_currentChanged(int index);

   void on_pushButton_nomad_browse_clicked();

private:
   Ui::CSetting *ui;
   void fillAstComList(QTreeWidget *list, const QList<urlItem_t> &strList, bool sun = false);
   void getAstComList(QTreeWidget *list, QList<urlItem_t> &strList);
};

#endif // CSETTING_H
