#ifndef SETTING_H
#define SETTING_H

#include <QtGui>

#include "dso_def.h"
#include "cmapview.h"
#include "cskpainter.h"

#define MRGB(r, g, b)          QColor(r, g, b).rgb()

typedef struct
{
  double fromFov;
  double mag;
} magRange_t;

typedef struct
{
  QString name;
  bool    italic;
  int     weight;
  int     size;
  QRgb    color;
} font_t;

#define  DSO_COL_NEBULA         0
#define  DSO_COL_BRIGHT_NEB     1
#define  DSO_COL_DARK_NEB       2
#define  DSO_COL_OPEN_CLS       3
#define  DSO_COL_GLOB_CLS       4
#define  DSO_COL_PLN_NEB        5
#define  DSO_COL_GALAXY         6
#define  DSO_COL_GAL_CLS        7
#define  DSO_COL_STAR           8
#define  DSO_COL_OTHER          9
#define  DSO_COL_COUNT         10

#define MAG_RNG_COUNT          10

#define FONT_DSO                0
#define FONT_STAR_PNAME         1
#define FONT_STAR_BAYER         2
#define FONT_STAR_FLAMS         3
#define FONT_PLN_SAT            4
#define FONT_PLANET             5
#define FONT_HORIZON            6
#define FONT_CONST              7
#define FONT_COMET              8
#define FONT_ASTER              9
#define FONT_DRAWING           10
#define FONT_TRACKING          11
#define FONT_EARTH_SHD         12
#define FONT_LUNAR_FEATURES    13
#define FONT_GRID              14
#define FONT_SATELLITE         15
#define FONT_COUNT             16

#define GRID_NONE               0
#define GRID_EQ                 1
#define GRID_ALL                2

typedef struct
{
  int  style;
  int  width;
  QRgb color;
} penStyle_t;

typedef struct
{
  int  type;           // GRID_xx
  QRgb color;
} setGrid_t;

typedef struct
{
  bool  bStatic;
  QRgb  staticColor;
  QRgb  dynamicColor[3];
  bool  useAltAzmOnly;
} setBk_t;

typedef struct
{
  QRgb  penColor;
  QRgb  brColor;
  QRgb  satColor;
  QRgb  satColorShd;
  QRgb  lunarFeatures;
  float phaseAlpha;
  int   plnRad;
  int   satRad;
  double jupGRSLon;
  double jupGRSYearDrift;
  double jupGRSDate;
  QString moonImage;
  bool    useCustomMoonTexture;
} planet_t;

typedef struct
{
  QRgb  color;
  int   radius;
  float plusMag;
  float maxMag;
} astCom_t;

typedef struct
{
  double propNamesFromFov;
  double bayerFromFov;
  bool   bayerPriority;
  bool   namePriority;
  bool   useSpectralTp;
  double flamsFromFov;
  double starSizeFactor;
  int    saturation;
  double properMotionYearVec;
  bool   showProperMotion;
} setStar_t;

typedef struct
{
  penStyle_t main;
  penStyle_t sec;
  penStyle_t bnd;
  QString   linesFile;
  QString   language;
} const_t;

typedef struct
{
  QRgb  color;
  bool  showDirections;
  int   alpha;
  bool  cb_hor_show_alt_azm;
  bool  hideTextureWhenMove;
} horizon_t;

typedef struct
{
  bool  bShow;
  bool  sameAsBkColor;
  QRgb  color;
  float light;
  float dark;
} milkyWay_t;

typedef struct
{
  QRgb  color;
} setDraw_t;

typedef struct
{
  QRgb  color;
} setTrack_t;

typedef struct
{
  bool   show;
  double fromFOV;
  double fromMag;
} setPpmxl_t;

typedef struct
{
  bool   show;
  double fromFOV;
  double fromMag;
} setUsno2_t;

typedef struct
{
  bool   show;
  double fromFOV;
  double fromMag;
} setUsnoB1_t;

typedef struct
{
  bool   show;
  double fromFOV;
  double fromMag;
} setURAT1_t;

typedef struct
{
  bool   show;
  double fromFOV;
  double fromMag;
} setUCAC4_t;

typedef struct
{
  bool   show;
  double fromFOV;
  double fromMag;
} setGsc_t;

typedef struct
{
  bool  show;
  QRgb  color;
  float alpha;
} setES_t;

typedef struct
{
  QRgb  color;
  double size;
} setSatellite_t;

typedef struct
{
  bool       dsoTypeShow[DSOT_COUNT];
  bool       dsoTypeShowAll[DSOT_COUNT];
  QRgb       dsoColors[DSO_COL_COUNT];
  int        dsoStyle[DSO_COL_COUNT];
  int        dsoWidth[DSO_COL_COUNT];
  QRgb       dsoShapeColor[3];
  QString    starBitmapName;
  magRange_t starRange[MAG_RNG_COUNT];
  magRange_t dsoRange[MAG_RNG_COUNT];
  double     dsoNoMagShapeFOV;
  double     dsoNoMagOtherFOV;
  QString    dsoFilter;

  setGrid_t  grid[SMCT_COUNT];
  bool       autoGrid;
  bool       showGridLabels;

  bool       smartLabels;

  QRgb       objSelectionColor;
  penStyle_t measurePoint;

  setBk_t    background;
  setStar_t  star;

  milkyWay_t milkyWay;
  horizon_t  hor;
  const_t    constellation;
  planet_t   planet;
  astCom_t   comet;
  astCom_t   aster;
  setSatellite_t satellite;
  setDraw_t  drawing;
  setTrack_t tracking;
  setPpmxl_t ppmxl;
  setUsno2_t  usno2;
  setUsnoB1_t usnob1;
  setURAT1_t  urat1;
  setUCAC4_t ucac4;
  setGsc_t   gsc;
  setES_t    es;
} setMap_t;

typedef struct
{
  bool tracking;
  bool drawing;
  bool events;
  bool mapPosition;
  bool dssImages;
} setAutosave_t;

typedef struct
{
  setMap_t      map;
  font_t        fonst[FONT_COUNT];
} setting_t;

extern setAutosave_t g_autoSave;

extern QFont setFonts[FONT_COUNT];
extern setting_t g_skSet;
extern QString   g_setName;

QFont setGetFonts(font_t *fnt);
void setSetFont(int no, CSkPainter *p);
void setSetFontColor(int no, CSkPainter *p);

bool setLoad(QString name, setting_t *set = &g_skSet);
bool setSave(QString name, setting_t *set = &g_skSet);
bool setRemove(QString name);

void setDefaultStarMagRanges(setting_t *set);

void setCreateFonts(void);
void setSetDefaultVal(void);
void restoreFromNightConfig(void);
void setNightConfig(void);
void setPrintConfig(void);
void restoreFromPrintConfig(void);
void setSetFontItem(int no, const QString &name, bool italic, int weight, int size, QRgb color);

#endif // SETTING_H
