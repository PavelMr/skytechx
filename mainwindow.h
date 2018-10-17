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
#include "chipsadjustment.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>

class CDSSCurveWidget;
class QPrinter;
class CHistogram;
class CHorEditorWidget;

class ItemDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  CStatusBar *statusBar;

  void fillQuickInfo(ofiItem_t *item, bool update = false);
  bool isQuickInfoTimeUpdate();
  ofiItem_t *getQuickInfo(void);
  void setToolBoxPage(int page);
  void updateDSS(bool refill = true);
  void checkDSS(void);
  void setShapeInfo(QString info);
  void updateTrackingMenu();
  void updateLunarInfo(const QString &desc, double lon, double lat, bool isValid);
  bool isLunarInfoTab();

  bool            m_bRealTime;
  bool            m_bRealTimeLapse;
  CMultiProgress *m_pcDSSProg;
  bool            m_slewButton;

  void centerSearchBox(bool bCenter);
  void removeQuickInfo(int type);
  void lfGetParam(lfParam_t *lfp);
  void lfSetParam(const lfParam_t *lfp);

  void saveAndExit();
  void timeDialogUpdate();

  void setToolbarIconSize();

  void checkNewVersion(bool forced);

  void enableReleaseObject(bool enable);

  void updateControlInfo();
  void setChartMode(int mode);
  void setRTC(bool start);
  bool getRTC();
  void fillHIPSSources();

  CMapView *getView();

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

  bool            m_noChangeFilterIndex;
  bool            m_checkVerForced;
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

  CHorEditorWidget *m_horizonEditor;

  CHistogram     *m_histogram;
  CHIPSAdjustment*m_hipsAdjustmentWidget;
  QDockWidget    *m_dockHipsAdjustment;
  QDockWidget    *m_dockHistogram;
  CTimeWidget    *m_timeWidget;
  CTimeDialog    *m_timeDialog;

  bool            m_isNormal;
  bool            m_noRecalculateView;
  QString         m_dlgProfileName;

  QNetworkAccessManager m_versionManager;

  QVector <double> m_raRates;
  QVector <double> m_decRates;

  QTextBrowser *m_webView;

private slots:
  void slotTelePlugTimer();

  void slotVersionFinished(QNetworkReply *reply);

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

  void on_actionSunrise_triggered();

  void on_actionSun_transit_triggered();

  void on_actionSunset_triggered();

  void on_actionMoonrise_triggered();

  void on_actionMoon_transit_triggered();

  void on_actionMoonset_triggered();

  void slotSearchPlanetTriggered();

  void on_actionSatellite_triggered();

  void on_actionSatellite_2_triggered();

  void on_actionSatellite_3_triggered(bool checked);

  void on_actionDSO_by_catalogue_triggered();

  void on_actionEpoch_J2000_0_toggled(bool arg1);

  void on_actionCheck_new_version_triggered();

  void on_pushButton_17_clicked();

  void on_actionHorizon_triggered(bool checked);

  void on_actionSatellite_chart_triggered();

  void on_actionRelease_notes_triggered();

  void on_actionShow_Hide_shading_planet_triggered(bool checked);

  void on_actionDonation_triggered();

  void on_actionText_2_triggered();

  void on_actionDSS_Manager_triggered();

  void on_pushButton_18_clicked();

  void on_pushButton_19_clicked();

  void on_pushButton_20_clicked();

  void on_pushButton_21_clicked();

  void on_actionObject_tracking_2_triggered(bool checked);

  void on_actionLook_backward_triggered();

  void on_action3D_Solar_system_triggered();

  void on_actionRelease_object_triggered();

  void on_tb_filter_clicked();

  void on_pushButton_22_clicked();

  void on_comboBox_3_currentIndexChanged(int index);

  void on_actionBinocular_triggered();

  void on_pushButton_23_clicked();

  void on_pushButton_28_clicked();

  void on_pushButton_29_clicked();

  void on_pushButton_27_clicked();

  void on_pushButton_25_clicked();

  void on_pushButton_24_clicked();

  void on_pushButton_26_clicked();

  void on_pushButton_30_clicked();

  void on_pushButton_31_clicked();

  void on_pushButton_32_clicked();

  void on_pushButton_33_clicked();

  void on_actionCenter_of_screen_triggered(bool checked);

  void on_actionFinder_FOV_triggered();

  void on_horizontalSlider_sliderReleased();

  void on_timeEdit_timeChanged(const QTime &time);

  void on_calendarWidget_selectionChanged();

  void on_actionMoonless_nights_triggered();

  void on_pushButton_34_clicked();

  void on_actionSearch_help_triggered();

  void on_pushButton_35_clicked();

  void on_actionPlanet_size_triggered();

  void on_actionShow_planet_axis_triggered(bool checked);

  void on_actionAdvanced_search_triggered();

  void on_actionSlew_telescope_to_screen_center_triggered();

  void on_actionGeocentric_triggered(bool checked);

  void on_pushButton_36_clicked();

  void on_cb_extInfo_toggled(bool checked);

  void on_pb_tc_right_pressed();

  void on_pb_tc_right_released();

  void on_pb_tc_left_pressed();

  void on_pb_tc_left_released();

  void on_pb_tc_up_pressed();

  void on_pb_tc_up_released();

  void on_pb_tc_down_pressed();

  void on_pb_tc_down_released();

  void on_pb_tp_stop_clicked();

  void on_pb_tc_find_clicked();

  void on_actionShow_local_meridian_triggered();

  void on_actionMeridian_triggered();

  void on_action_zoom_100_triggered();

  void on_pushButton_unselect_clicked();

  void on_tb_histogram_clicked();  

  void on_actionShow_meteor_showers_triggered(bool checked);

  void on_actionLunar_features_triggered();

  void on_actionHIPS_toggled(bool arg1);

  void on_actionHEALPix_grid_toggled(bool arg1);

  void on_actionHIPS_billinear_toggled(bool arg1);

  void on_actionHIPS_properties_triggered();

  void on_actionVO_Catalogue_triggered();

  void on_actionShow_Hide_VO_Catalogue_triggered(bool checked);

  void on_actionSunspots_triggered();

  void on_actionVariable_stars_triggered();

  void on_actionTelescope_frame_triggered();

  void on_actionTwilight_2_triggered();

  void on_actionLunar_phase_triggered();

  void on_toolButton_2_clicked();

  void on_actionClear_all_tracking_paths_triggered();

  void on_pushButton_37_clicked();

  void checkSlewButton();

  void on_cb_lf_labels_toggled(bool checked);

  void on_actionRendering_triggered();

  void on_actionAsterism_triggered(bool checked);

  void on_actionHiPS_Adjustment_triggered(bool checked);

  void on_actionShortcut_settings_triggered();

private:
  Ui::MainWindow *ui;
  CDSOCatalogue *m_DSOCatalogueDlg;
  CDSSCurveWidget *m_curve;
  event_t m_currentEvent;
  QMenu *m_HIPSMenu;
  QAction *m_actionHIPSNone;
  QString m_HIPSTmpUrl;
  QString m_HIPSProperties;
  QString m_HIPSUrl;
  QToolButton *m_hipsToolButton;
  int m_settingTab;

  void setTitle();
  void restoreDSSList();
  void saveDSSList();
  void updateHomeLocation();
  int getTelescopeSpeed();    

  QList <QAction *> m_homeLocActions;

public slots:
  void repaintMap();
  void slotFilterChanged();
  void slotOpenWebHelp();
  void slotPrintPreview(QPrinter*);
  void slotDsoCenter();
  void slotStatusBarDoubleClick(int id);
  void onTreeViewDSSContextMenuRequested(QPoint pt);
  void slotDownloadError(QString str);
  void slotDockBarFilter(bool vis);
  void slotDockBarVis(bool vis);
  void slotTimeVis(bool vis);
  void slotTeleVis(bool vis);
  void slotTimeDialogVis(bool vis);
  void slotHistogramVis(bool vis);
  void slotHIPSAdjustmentVis(bool vis);
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
  void slotTimeSliderUpdate();
  void slotCalendaryUpdate();
  void slotHelpClick(const QUrl &url);

signals:
  void sigMagLock(bool checked);
  void sigFlipX(bool checked);
  void sigFlipY(bool checked);

protected slots:
  void slotPluginError();  
  void slotHIPS();
  void slotHIPSPropertiesDone(QNetworkReply::NetworkError error, const QString &errorString);
};

extern MainWindow *pcMainWnd;

#endif // MAINWINDOW_H
