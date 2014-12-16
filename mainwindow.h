#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cstatusbar.h"
#include "cobjfillinfo.h"
#include "ctristatebutton.h"
#include "cmultiprogress.h"
#include "clineeditcomp.h"
#include "ceventsearch.h"
#include "clunarfeatures.h"
#include "ctimewidget.h"
#include "ctimedialog.h"
#include "cdsocatalogue.h"

class QPrinter;

namespace Ui {
class MainWindow;
}


class CDSSOpenDialog : public QFileDialog
{
  Q_OBJECT

public:
  CDSSOpenDialog(QWidget *parent = 0,
                 const QString &caption = QString(),
                 const QString &directory = QString(),
                 const QString &filter = QString());
  int getSize();
protected:
  QComboBox *m_sizeComboBox;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  CStatusBar *statusBar;

  void fillQuickInfo(ofiItem_t *item, bool scroll = false);
  bool isQuickInfoTimeUpdate();
  ofiItem_t *getQuickInfo(void);
  void repaintMap(void);
  void setToolBoxPage(int page);
  void updateDSS(bool refill = true);
  void checkDSS(void);
  void setShapeInfo(QString info);

  bool            m_bRealTime;
  bool            m_bRealTimeLapse;
  CMultiProgress *m_pcDSSProg;

  void centerSearchBox(bool bCenter);
  void removeQuickInfo(int type);
  void lfGetParam(lfParam_t *lfp);

  void saveAndExit();
  void timeDialogUpdate();

protected:

  void changeEvent(QEvent *e);
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *);
  bool eventFilter(QObject *obj, QEvent *event);

  void refillEI();
  QString getEventDesc(event_t *e);
  void fillEventInfo(event_t *e, QString title, bool warning);
  bool exitQuestion();

  QList <QStandardItem *> createEIRow(event_t *e, QString c1, QString c2, QString c3, QVariant jd, QVariant ra, QVariant dec, QVariant zoom);

  int getCurDSS(void);

  QString         m_evNames[8];
  QSpinBox       *m_timeMul;
  CLineEditComp  *m_search;
  QSpinBox       *m_timeLapseMul;
  int             m_timeLapseUpdate;

  CTriStateButton *btGrid1;
  CTriStateButton *btGrid2;
  CTriStateButton *btGrid3;

  QElapsedTimer   m_realElapsedTimerLapse;
  QTimer          m_realTimerLapse;
  QTimer          m_realTimer;
  QStringList     m_wordList;

  CTimeWidget    *m_timeWidget;
  CTimeDialog    *m_timeDialog;
  bool            m_isNormal;
  bool            m_noRecalculateView;
  QString         m_dlgProfileName;

private slots:
  void on_action_Exit_2_triggered();

  void on_action_About_triggered();

  void on_inc_star_mag_triggered();

  void on_dec_star_mag_triggered();

  void on_actionStar_mag_lock_toggled(bool arg1);


  void on_actionFlipX_toggled(bool arg1);

  void on_actionFlipY_toggled(bool arg1);

  void on_actionInc_dso_mag_triggered();

  void on_actionDec_dso_mag_triggered();

  void on_action_zoom_1_triggered();

  void on_action_zoom_5_triggered();

  void on_action_zoom_10_triggered();

  void on_action_zoom_20_triggered();

  void on_action_zoom_45_triggered();

  void on_action_zoom_90_triggered();

  void on_actionExport_map_to_image_triggered();

  void on_actionPrint_triggered();

  void on_actionCopy_map_to_clipboard_triggered();

  void on_actionAtlas_mode_Pole_Up_triggered();

  void on_actionHorizon_mode_Zenith_up_triggered();

  void on_actionSet_JD_triggered();

  void on_actionSet_local_time_triggered();

  void on_actionSet_UTC_time_triggered();

  void on_actionSet_current_time_triggered();

  void on_actionDeltaT_triggered();

  void on_actionShow_sidebar_toggled(bool arg1);

  void on_action_Last_search_object_triggered();

  void on_pushButton_clicked();

  void on_actionYPlus_triggered();

  void on_actionYMinus_triggered();

  void on_actionMPlus_triggered();

  void on_actionMMinus_triggered();

  void on_actionDPlus_triggered();

  void on_actionDMinus_triggered();

  void on_actionCurLocTime_triggered();

  void on_actionSDPlus_triggered();

  void on_actionSDMinus_triggered();

  void on_actionHPlus_triggered();

  void on_actionHMinus_triggered();

  void on_actionMinPlus_triggered();

  void on_actionMinMinus_triggered();

  void on_actionSPlus_triggered();

  void on_actionSMinus_triggered();

  void on_actionStar_by_proper_name_triggered();

  void on_actionDOS_by_common_name_triggered();

  void on_actionSearch_triggered();

  void on_actionEcliptic_triggered();

  void on_actionGalactic_coordinates_triggered();

  void on_actionPosition_triggered();

  void on_actionNorth_triggered();

  void on_actionSouth_triggered();

  void on_actionEast_triggered();

  void on_actionWest_triggered();

  void on_actionZenith_triggered();

  void on_actionAutomatic_grid_triggered();

  void on_actionRealtime_triggered(bool checked);

  void on_actionDay_Night_triggered();

  void on_actionSOHO_Sun_images_triggered();

  void on_actionPlanet_visibility_triggered();

  void on_actionMoon_calendary_triggered();

  void on_actionSetting_triggered();

  void on_actionOpen_DSS_file_triggered();

  void on_pushButton_2_clicked();

  void on_actionConnect_device_triggered();

  void on_actionSelect_world_location_triggered();

  void on_actionDisconnect_triggered();

  void on_actionFind_telescope_triggered();

  void on_actionTelescope_triggered();

  void on_actionStandard_cross_triggered();

  void on_actionSelect_triggered();

  void on_actionVery_fast_100ms_triggered();

  void on_actionFast_250ms_triggered();

  void on_actionSlow_500ms_triggered();

  void on_actionVery_slow_1000ms_triggered();

  void on_treeView_clicked(const QModelIndex &index);

  void on_pushButton_dssC_clicked();

  void on_pushButton_dssCZ_clicked();

  void on_horizontalSlider_br_valueChanged(int value);

  void on_horizontalSlider_con_valueChanged(int value);

  void on_horizontalSlider_gm_valueChanged(int value);

  void on_checkBox_inv_clicked(bool checked);

  void on_checkBox_aa_clicked(bool checked);

  void on_pushButton_dss_reset_clicked();

  void on_pushButton_dss_reset_all_clicked();

  void on_pushButton_dss_all_clicked();

  void on_checkBox_toggled(bool checked);

  void on_pushButton_3_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_dssDA_clicked();

  void on_actionAsteroids_triggered();

  void on_actionAsteroid_triggered();

  void on_actionSave_time_mark_triggered();

  void on_pushButton_4_clicked();

  void on_actionRestore_time_mark_triggered();

  void on_actionDay_event_triggered();

  void on_actionStars_triggered(bool checked);

  void on_actionConstellation_lines_triggered(bool checked);

  void on_actionConstellation_boundaries_triggered(bool checked);

  void on_actionDSO_triggered(bool checked);

  void on_actionMilkyway_triggered(bool checked);

  void on_actionSolar_system_triggered(bool checked);

  void on_actionDSO_Shapes_triggered(bool checked);

  void on_actionSatelitte_daily_events_triggered();

  void on_actionGrids_triggered(bool checked);

  void on_actionShow_all_triggered();

  void on_actionStar_in_Tycho_catalogue_triggered();

  void on_actionSearch_events_triggered();


  void on_comboBox_2_currentIndexChanged(int index);

  void on_comboBox_currentIndexChanged(int index);

  void on_treeView_2_doubleClicked(const QModelIndex &index);

  void on_treeView_3_clicked(const QModelIndex &index);

  void on_actionAntialiasing_triggered(bool checked);

  void on_actionSymbol_Real_planet_rendering_triggered(bool checked);

  void on_actionComets_triggered();

  void on_actionComet_triggered();

  void on_actionObject_tracking_triggered();

  void on_checkBox_4_toggled(bool checked);

  void on_hr_lf_detail_valueChanged(int value);

  void on_pushButton_5_clicked();

  void on_hr_lf_detail_2_valueChanged(int value);

  void on_actionTelescope_FOV_triggered();

  void on_actionLegends_triggered(bool checked);

  void on_actionComets_2_triggered(bool checked);

  void on_actionDoube_star_triggered();

  void on_action_Asteroids1_triggered(bool checked);

  void on_actionPlanets_Sun_Moon_triggered();

  void on_actionLabels_triggered(bool checked);

  void on_actionTelrad_triggered();

  void on_actionText_triggered();

  void on_actionClear_map_triggered();

  void on_actionHide_all_triggered();

  void on_actionFrame_field_triggered();

  void on_actionCenter_edited_object_triggered();

  void on_actionFind_edited_object_triggered();

  void on_actionTime_window_triggered(bool checked);

  void on_checkBox_shape_toggled(bool checked);

  void on_pushButton_7_clicked();

  void on_pushButton_6_clicked();

  void on_actionShow_full_screen_triggered(bool checked);

  void on_actionChnage_style_triggered();

  void on_actionTPStop_triggered();

  void on_actionMeasure_point_triggered();

  void on_actionTelescope_window_triggered(bool checked);

  void on_actionCircle_triggered();

  void on_checkBox_shape_2_toggled(bool checked);

  void on_actionEphemeride_list_triggered();

  void on_actionBilinear_interpolation_triggered(bool checked);

  void on_actionPolaris_Hour_Angle_triggered();

  void on_treeView_2_clicked(const QModelIndex &index);

  void on_pushButton_10_clicked();

  void on_actionTime_lapse_toggled(bool arg1);

  void on_actionDeep_sky_galery_triggered();

  void on_actionSet_horizon_triggered();

  void on_actionKeyboard_reference_triggered();

  void on_checkBox_5_clicked(bool checked);

  void on_actionChange_language_triggered();

  void on_actionReset_magnitude_triggered();

  void on_actionTime_Lapse_prefs_triggered();

  void on_actionShow_help_triggered();

  void on_pushButton_12_clicked();

  void on_pushButton_11_clicked();

  void on_actionNight_mode_triggered(bool checked);

  void on_lv_quickInfo_customContextMenuRequested(const QPoint &pos);

  void on_pushButton_8_clicked();

  void on_checkBox_lfDiam_toggled(bool checked);

  void on_actionSolar_system_2_triggered();

  void on_action_Constellation_triggered();

  void on_actionTime_dialog_triggered(bool checked);

  void on_actionList_of_drawings_triggered();

  void on_pushButton_13_clicked();

  void on_pushButton_14_clicked();

  void on_pushButton_15_clicked();

  void on_actionLock_FOV_toggled(bool arg1);

  void on_cb_showDSS_FN_toggled(bool checked);

  void on_actionPlanet_altitude_triggered();

  void on_actionActual_weather_triggered();

  void on_actionGrid_label_triggered();

  void on_actionDrawings_triggered(bool checked);

  void on_actionTip_of_the_day_triggered();

  void on_actionDeep_Sky_Objects_triggered(bool checked);

  void on_actionPrint_preview_triggered();

  void on_pushButton_16_clicked();

  void on_actionCustom_DSO_triggered();

  void on_actionGo_to_SkytechX_home_page_triggered();

private:
  Ui::MainWindow *ui;
  CDSOCatalogue *m_DSOCatalogueDlg;

  void setTitle();
public slots:
  void slotPrintPreview(QPrinter*);
  void slotDsoCenter();
  void slotStatusBarDoubleClick(int id);
  void onTreeViewDSSContextMenuRequested(QPoint pt);
  void slotDownloadError(QString str);
  void slotDockBarVis(bool vis);
  void slotTimeVis(bool vis);
  void slotTeleVis(bool vis);
  void slotTimeDialogVis(bool vis);
  void slotSearchDone(void);
  void slotSearchChange(QString str);
  void slotGrid1(void);
  void slotGrid2(void);
  void slotGrid3(void);
  void slotRealTime(void);
  void slotRealTimeLapse(void);
  void slotDeleteDSSItem(void);
  void slotSelectionChangedEI( const QItemSelection &, const QItemSelection & );
  void slotDeleteEI(void);
  void slotDataChange(const QModelIndex &, const QModelIndex &);
  void slotDSSChange(const QModelIndex &i, const QModelIndex &i2);
  void slotDrawingChange(bool bEdited, bool isEmpty);
  void slotTimeUpdate(void);
  void slotCheckFirstTime();

signals:
  void sigMagLock(bool checked);
  void sigFlipX(bool checked);
  void sigFlipY(bool checked);

protected slots:
  void slotPluginError();
};

extern MainWindow *pcMainWnd;

#endif // MAINWINDOW_H
