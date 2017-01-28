#include "skcore.h"
#include "setting.h"
#include "cskpainter.h"
#include "cstarrenderer.h"

setAutosave_t g_autoSave;
QString       g_setName;
setting_t     g_skSet;
QFont         setFonts[FONT_COUNT];

setting_t     skSetTmp;
setting_t     skPrintTmp;
bool          g_nightConfig = false;

static QString       qssStyle;
extern QApplication *g_pApp;


static double starMagnitudes[10][2] = {
                                        {D2R(90), 5.5},
                                        {D2R(75), 6.0},
                                        {D2R(50), 7.0},
                                        {D2R(30), 8.0},
                                        {D2R(20), 9.0},
                                        {D2R(10), 10.0},
                                        {D2R(5),  11.3},
                                        {D2R(2.5), 12.5},
                                        {D2R(1), 15.0},
                                        {D2R(0.25), 20.0}
                                      };

bool setRemove(QString name)
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/" + name + ".dat");

  return(f.remove());
}

void static writeVal(const QString &key, const QVariant &value, QDataStream &ds)
{
  ds << key << value;
}

QVariant static readVal(const QString &key, const QVariant &defaultValue, QMap <QString, QVariant> &map)
{
  if (map.contains(key))
  {
    return map[key];
  }

  //qDebug() << "Not found" << key << defaultValue;

  return defaultValue;
}

static font_t readFont(const QString &key, QMap <QString, QVariant> &map, const QString &name, bool italic, int weight, int size, QRgb color)
{
  font_t font;

  font.name = readVal(key + ".name", name, map).toString();
  font.color = readVal(key + ".color" , color, map).toUInt();
  font.italic = readVal(key + ".italic" , italic, map).toBool();
  font.size = readVal(key + ".size" , size, map).toInt();
  font.weight = readVal(key + ".weight" , weight, map).toInt();

  return font;
}

static void writeFont(const QString &key, font_t *font, QDataStream &ds)
{
  writeVal(key + ".name" , font->name, ds);
  writeVal(key + ".color" , font->color, ds);
  writeVal(key + ".italic" , font->italic, ds);
  writeVal(key + ".size" , font->size, ds);
  writeVal(key + ".weight" , font->weight, ds);
}

//////////////////////////////////////////
bool setSave(QString name, setting_t *set)
//////////////////////////////////////////
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/" + name + ".dat");
  QDataStream ds(&f);

  qDebug() << "saving profile" << f.fileName();

  if (!f.open(SkFile::WriteOnly))
  {
    return false;
  }

  writeVal("VERSION", 1, ds);

  // fonts
  writeFont("font.dso", &set->fonst[FONT_DSO], ds);
  writeFont("font.star.pname", &set->fonst[FONT_STAR_PNAME], ds);
  writeFont("font.star.bayer", &set->fonst[FONT_STAR_BAYER], ds);
  writeFont("font.star.flam", &set->fonst[FONT_STAR_FLAMS], ds);
  writeFont("font.pln.sat", &set->fonst[FONT_PLN_SAT], ds);
  writeFont("font.planet", &set->fonst[FONT_PLANET], ds);
  writeFont("font.horizon", &set->fonst[FONT_HORIZON], ds);
  writeFont("font.const", &set->fonst[FONT_CONST], ds);
  writeFont("font.comet", &set->fonst[FONT_COMET], ds);
  writeFont("font.aster", &set->fonst[FONT_ASTER], ds);
  writeFont("font.drawing", &set->fonst[FONT_DRAWING], ds);
  writeFont("font.tracking", &set->fonst[FONT_TRACKING], ds);
  writeFont("font.earth.shd", &set->fonst[FONT_EARTH_SHD], ds);
  writeFont("font.lunar.features", &set->fonst[FONT_LUNAR_FEATURES], ds);
  writeFont("font.grid", &set->fonst[FONT_GRID], ds);
  writeFont("font.satellite", &set->fonst[FONT_SATELLITE], ds);
  writeFont("font.shower", &set->fonst[FONT_SHOWER], ds);
  writeFont("font.star.vars", &set->fonst[FONT_STAR_VARS], ds);

  // earth shadow
  writeVal("map.es.show", set->map.es.show, ds);
  writeVal("map.es.color", set->map.es.color, ds);
  writeVal("map.es.alpha", set->map.es.alpha, ds);

  // stars
  writeVal("map.star.propNamesFromFov", set->map.star.propNamesFromFov, ds);
  writeVal("map.star.bayerFromFov", set->map.star.bayerFromFov, ds);
  writeVal("map.star.flamsFromFov", set->map.star.flamsFromFov, ds);
  writeVal("map.star.bayerPriority", set->map.star.bayerPriority, ds);
  writeVal("map.star.namePriority", set->map.star.namePriority, ds);
  writeVal("map.star.useSpectralTp", set->map.star.useSpectralTp, ds);
  writeVal("map.star.starSizeFactor", set->map.star.starSizeFactor, ds);
  writeVal("map.star.saturation", set->map.star.saturation, ds);
  writeVal("map.star.properMotionYearVec", set->map.star.properMotionYearVec, ds);
  writeVal("map.star.showProperMotion", set->map.star.showProperMotion, ds);
  writeVal("map.star.useProperMotion", set->map.star.useProperMotion, ds);
  writeVal("map.star.showGlow", set->map.star.showGlow, ds);
  writeVal("map.star.glowAlpha", set->map.star.glowAlpha, ds);
  writeVal("map.star.varsFromFov", set->map.star.varsFromFov, ds);
  writeVal("map.star.showVarLabels", set->map.star.showVarLabels, ds);

  writeVal("map.starBitmapName", set->map.starBitmapName, ds);

  // satellite
  writeVal("map.satellite.color", set->map.satellite.color, ds);
  writeVal("map.satellite.size", set->map.satellite.size, ds);

  // horizon
  writeVal("map.hor.color", set->map.hor.color, ds);
  writeVal("map.hor.alpha", set->map.hor.alpha, ds);
  writeVal("map.hor.cb_hor_show_alt_azm", set->map.hor.cb_hor_show_alt_azm, ds);
  writeVal("map.hor.showDirections", set->map.hor.showDirections, ds);
  writeVal("map.hor.hideTextureWhenMove", set->map.hor.hideTextureWhenMove, ds);

  // dso
  writeVal("map.dsoColor_nebula", set->map.dsoColors[DSO_COL_NEBULA], ds);
  writeVal("map.dsoStyle_nebula", set->map.dsoStyle[DSO_COL_NEBULA], ds);
  writeVal("map.dsoWidth_nebula", set->map.dsoWidth[DSO_COL_NEBULA], ds);

  writeVal("map.dsoColor_bright.neb", set->map.dsoColors[DSO_COL_BRIGHT_NEB], ds);
  writeVal("map.dsoStyle_bright.neb", set->map.dsoStyle[DSO_COL_BRIGHT_NEB], ds);
  writeVal("map.dsoWidth_bright.neb", set->map.dsoWidth[DSO_COL_BRIGHT_NEB], ds);

  writeVal("map.dsoColor_dark.neb", set->map.dsoColors[DSO_COL_DARK_NEB], ds);
  writeVal("map.dsoStyle_dark.neb", set->map.dsoStyle[DSO_COL_DARK_NEB], ds);
  writeVal("map.dsoWidth_dark.neb", set->map.dsoWidth[DSO_COL_DARK_NEB], ds);

  writeVal("map.dsoColor_open.cls", set->map.dsoColors[DSO_COL_OPEN_CLS], ds);
  writeVal("map.dsoStyle_open.cls", set->map.dsoStyle[DSO_COL_OPEN_CLS], ds);
  writeVal("map.dsoWidth_open.cls", set->map.dsoWidth[DSO_COL_OPEN_CLS], ds);

  writeVal("map.dsoColor_glob.cls", set->map.dsoColors[DSO_COL_GLOB_CLS], ds);
  writeVal("map.dsoStyle_glob.cls", set->map.dsoStyle[DSO_COL_GLOB_CLS], ds);
  writeVal("map.dsoWidth_glob.cls", set->map.dsoWidth[DSO_COL_GLOB_CLS], ds);

  writeVal("map.dsoColor_pln.neb", set->map.dsoColors[DSO_COL_PLN_NEB], ds);
  writeVal("map.dsoStyle_pln.neb", set->map.dsoStyle[DSO_COL_PLN_NEB], ds);
  writeVal("map.dsoWidth_pln.neb", set->map.dsoWidth[DSO_COL_PLN_NEB], ds);

  writeVal("map.dsoColor_galaxy", set->map.dsoColors[DSO_COL_GALAXY], ds);
  writeVal("map.dsoStyle_galaxy", set->map.dsoStyle[DSO_COL_GALAXY], ds);
  writeVal("map.dsoWidth_galaxy", set->map.dsoWidth[DSO_COL_GALAXY], ds);

  writeVal("map.dsoColor_gal.cls", set->map.dsoColors[DSO_COL_GAL_CLS], ds);
  writeVal("map.dsoStyle_gal.cls", set->map.dsoStyle[DSO_COL_GAL_CLS], ds);
  writeVal("map.dsoWidth_gal.cls", set->map.dsoWidth[DSO_COL_GAL_CLS], ds);

  writeVal("map.dsoColor_star", set->map.dsoColors[DSO_COL_STAR], ds);
  writeVal("map.dsoStyle_star", set->map.dsoStyle[DSO_COL_STAR], ds);
  writeVal("map.dsoWidth_star", set->map.dsoWidth[DSO_COL_STAR], ds);

  writeVal("map.dsoColor_star", set->map.dsoColors[DSO_COL_STAR], ds);
  writeVal("map.dsoStyle_star", set->map.dsoStyle[DSO_COL_STAR], ds);
  writeVal("map.dsoWidth_star", set->map.dsoWidth[DSO_COL_STAR], ds);

  writeVal("map.dsoColor_other", set->map.dsoColors[DSO_COL_OTHER], ds);
  writeVal("map.dsoStyle_other", set->map.dsoStyle[DSO_COL_OTHER], ds);
  writeVal("map.dsoWidth_other", set->map.dsoWidth[DSO_COL_OTHER], ds);

  writeVal("map.dsoShapeColor0", set->map.dsoShapeColor[0], ds);
  writeVal("map.dsoShapeColor1", set->map.dsoShapeColor[1], ds);
  writeVal("map.dsoShapeColor2", set->map.dsoShapeColor[2], ds);
  writeVal("map.dsoShapeAlpha", set->map.dsoShapeAlpha, ds);

  writeVal("map.dsoNoMagShapeFOV", set->map.dsoNoMagShapeFOV, ds);
  writeVal("map.dsoNoMagOtherFOV", set->map.dsoNoMagOtherFOV, ds);
  writeVal("map.dsoFadeTo", set->map.dsoFadeTo, ds);
  writeVal("map.dsoNoMagFadeFOV", set->map.dsoNoMagFadeFOV, ds);

  writeVal("map.dsoFilter", set->map.dsoFilter, ds);
  writeVal("map.dsoFilterType", set->map.dsoFilterType, ds);

  for (int i = 0; i < DSOT_COUNT; i++)
  {
    writeVal("map.dsoTypeShow" + QString::number(i), set->map.dsoTypeShow[i], ds);
    writeVal("map.dsoTypeShowAll" + QString::number(i), set->map.dsoTypeShowAll[i], ds);
  }

  for (int i = 0; i < 10; i++)
  {
    writeVal("map.starRangeFOV" + QString::number(i), set->map.starRange[i].fromFov, ds);
    writeVal("map.starRangeMag" + QString::number(i), set->map.starRange[i].mag, ds);
  }

  for (int i = 0; i < 10; i++)
  {
    writeVal("map.dsoRangeFOV" + QString::number(i), set->map.dsoRange[i].fromFov, ds);
    writeVal("map.dsoRangeMag" + QString::number(i), set->map.dsoRange[i].mag, ds);
  }

  // grids
  writeVal("map.grid.ra.dec.type", set->map.grid[SMCT_RA_DEC].type, ds);
  writeVal("map.grid.ra.dec.color", set->map.grid[SMCT_RA_DEC].color, ds);

  writeVal("map.grid.alt.azm.type", set->map.grid[SMCT_ALT_AZM].type, ds);
  writeVal("map.grid.alt.azm.color", set->map.grid[SMCT_ALT_AZM].color, ds);

  writeVal("map.grid.ecl.type", set->map.grid[SMCT_ECL].type, ds);
  writeVal("map.grid.ecl.color", set->map.grid[SMCT_ECL].color, ds);

  writeVal("map.grid.gal.type", set->map.grid[SMCT_GAL].type, ds);
  writeVal("map.grid.gal.color", set->map.grid[SMCT_GAL].color, ds);

  writeVal("map.grid.auto", set->map.autoGrid, ds);
  writeVal("map.grid.show.labels", set->map.showGridLabels, ds);

  writeVal("map.grid.show.showMeridian", set->map.showMeridian, ds);
  writeVal("map.grid.show.meridianColor", set->map.meridianColor, ds);

  // background
  writeVal("map.background.bStatic", set->map.background.bStatic, ds);
  writeVal("map.background.staticColor", set->map.background.staticColor, ds);

  qDebug() << "write" << set->map.background.staticColor;
  qDebug() << g_skSet.map.constellation.language;

  writeVal("map.background.dynamicColor0", set->map.background.dynamicColor[0], ds);
  writeVal("map.background.dynamicColor1", set->map.background.dynamicColor[1], ds);
  writeVal("map.background.dynamicColor2", set->map.background.dynamicColor[2], ds);
  writeVal("map.background.useAltAzmOnly", set->map.background.useAltAzmOnly, ds);

  // constellation
  writeVal("map.constellation.main.color", set->map.constellation.main.color, ds);
  writeVal("map.constellation.main.style", set->map.constellation.main.style, ds);
  writeVal("map.constellation.main.width", set->map.constellation.main.width, ds);

  writeVal("map.constellation.sec.color", set->map.constellation.sec.color, ds);
  writeVal("map.constellation.sec.style", set->map.constellation.sec.style, ds);
  writeVal("map.constellation.sec.width", set->map.constellation.sec.width, ds);

  writeVal("map.constellation.bnd.color", set->map.constellation.bnd.color, ds);
  writeVal("map.constellation.bnd.style", set->map.constellation.bnd.style, ds);
  writeVal("map.constellation.bnd.width", set->map.constellation.bnd.width, ds);

  writeVal("map.constellation.linesFile", set->map.constellation.linesFile, ds);
  writeVal("map.constellation.language", set->map.constellation.language, ds);
  writeVal("map.constellation.longNames", set->map.constellation.longNames, ds);
  writeVal("map.constellation.showNames", set->map.constellation.showNames, ds);

  // measure point
  writeVal("map.measurePoint.color", set->map.measurePoint.color, ds);
  writeVal("map.measurePoint.style", set->map.measurePoint.style, ds);
  writeVal("map.measurePoint.width", set->map.measurePoint.width, ds);

  // planets
  writeVal("map.planet.penColor", set->map.planet.penColor, ds);
  writeVal("map.planet.brColor", set->map.planet.brColor, ds);
  writeVal("map.planet.satColor", set->map.planet.satColor, ds);
  writeVal("map.planet.satColorShd", set->map.planet.satColorShd, ds);
  writeVal("map.planet.lunarFeatures", set->map.planet.lunarFeatures, ds);
  writeVal("map.planet.phaseAlpha", set->map.planet.phaseAlpha, ds);
  writeVal("map.planet.plnRad", set->map.planet.plnRad, ds);
  writeVal("map.planet.satRad", set->map.planet.satRad, ds);
  writeVal("map.planet.jupGRSLon", set->map.planet.jupGRSLon, ds);
  writeVal("map.planet.jupGRSYearDrift", set->map.planet.jupGRSYearDrift, ds);
  writeVal("map.planet.jupGRSDate", set->map.planet.jupGRSDate, ds);
  writeVal("map.planet.moonImage", set->map.planet.moonImage, ds);
  writeVal("map.planet.useCustomMoonTexture", set->map.planet.useCustomMoonTexture, ds);

  // shower
  writeVal("map.shower.bShowAll", set->map.shower.bShowAll, ds);
  writeVal("map.shower.daysBeforeAfterDate", set->map.shower.daysBeforeAfterDate, ds);
  writeVal("map.shower.scale", set->map.shower.scale, ds);
  writeVal("map.shower.color", set->map.shower.color, ds);
  writeVal("map.shower.nonActiveColor", set->map.shower.nonActiveColor, ds);

  // comet & aster.
  writeVal("map.comet.real", set->map.comet.real, ds);
  writeVal("map.comet.color", set->map.comet.color, ds);
  writeVal("map.comet.radius", set->map.comet.radius, ds);
  writeVal("map.comet.plusMag", set->map.comet.plusMag, ds);
  writeVal("map.comet.maxMag", set->map.comet.maxMag, ds);

  writeVal("map.aster.color", set->map.aster.color, ds);
  writeVal("map.aster.radius", set->map.aster.radius, ds);
  writeVal("map.aster.plusMag", set->map.aster.plusMag, ds);
  writeVal("map.aster.maxMag", set->map.aster.maxMag, ds);

  // milky way
  writeVal("map.milkyWay.bShow", set->map.milkyWay.bShow, ds);
  writeVal("map.milkyWay.light", set->map.milkyWay.light, ds);
  writeVal("map.milkyWay.dark", set->map.milkyWay.dark, ds);
  writeVal("map.milkyWay.darkest", set->map.milkyWay.darkest, ds);
  writeVal("map.milkyWay.sameAsBkColor", set->map.milkyWay.sameAsBkColor, ds);
  writeVal("map.milkyWay.color", set->map.milkyWay.color, ds);

  // drawing
  writeVal("map.drawing.color", set->map.drawing.color, ds);

  // object selection
  writeVal("map.objSelectionColor", set->map.objSelectionColor, ds);

  // tracking
  writeVal("map.tracking.color", set->map.tracking.color, ds);

  // USNO2
  writeVal("map.usno2.show", set->map.usno2.show, ds);
  writeVal("map.usno2.fromFOV", set->map.usno2.fromFOV, ds);
  writeVal("map.usno2.fromMag", set->map.usno2.fromMag, ds);

  // URAT1
  writeVal("map.urat1.show", set->map.urat1.show, ds);
  writeVal("map.urat1.fromFOV", set->map.urat1.fromFOV, ds);
  writeVal("map.urat1.fromMag", set->map.urat1.fromMag, ds);

  // USNO B1
  writeVal("map.usnob1.show", set->map.usnob1.show, ds);
  writeVal("map.usnob1.fromFOV", set->map.usnob1.fromFOV, ds);
  writeVal("map.usnob1.fromMag", set->map.usnob1.fromMag, ds);

  // UCAC4
  writeVal("map.ucac4.show", set->map.ucac4.show, ds);
  writeVal("map.ucac4.fromFOV", set->map.ucac4.fromFOV, ds);
  writeVal("map.ucac4.fromMag", set->map.ucac4.fromMag, ds);

  // PPMXL
  writeVal("map.ppmxl.show", set->map.ppmxl.show, ds);
  writeVal("map.ppmxl.fromFOV", set->map.ppmxl.fromFOV, ds);
  writeVal("map.ppmxl.fromMag", set->map.ppmxl.fromMag, ds);

  // NOMAD
  writeVal("map.nomad.show", set->map.nomad.show, ds);
  writeVal("map.nomad.fromFOV", set->map.nomad.fromFOV, ds);
  writeVal("map.nomad.fromMag", set->map.nomad.fromMag, ds);

  // GSC
  writeVal("map.gsc.show", set->map.gsc.show, ds);
  writeVal("map.gsc.fromFOV", set->map.gsc.fromFOV, ds);
  writeVal("map.gsc.fromMag", set->map.gsc.fromMag, ds);

  writeVal("map.smartLabels", set->map.smartLabels, ds);

  return true;
}


//////////////////////////////////////////
bool setLoad(QString name, setting_t *set)
//////////////////////////////////////////
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/" + name + ".dat");
  QDataStream ds(&f);
  QMap <QString, QVariant> tMap;

  if (name.isEmpty())
  {
    f.setFileName("");
    qDebug() << "reading default profile";
  }
  else
  {
    qDebug() << "reading profile" << f.fileName();
  }

  if (f.open(SkFile::ReadOnly))
  {
    do
    {
      QString key;
      QVariant val;

      ds >> key;
      ds >> val;

      tMap[key] = val;

    } while (!ds.atEnd());

    if (1 != readVal("VERSION", 0, tMap).toInt())
    {
      return false;
    }
  }

  // fonts
  set->fonst[FONT_DSO] = readFont("font.dso", tMap, "arial", false, QFont::Normal, 14, MRGB(100, 150, 100));
  set->fonst[FONT_STAR_PNAME] = readFont("font.star.pname", tMap, "arial", false, QFont::Normal, 14, MRGB(200, 200, 200));
  set->fonst[FONT_STAR_BAYER] = readFont("font.star.bayer", tMap, "symbol", false, QFont::Normal, 14, MRGB(200, 250, 200));
  set->fonst[FONT_STAR_FLAMS] = readFont("font.star.flam", tMap, "arial", false, QFont::Normal, 14, MRGB(250, 200, 200));
  set->fonst[FONT_PLN_SAT] = readFont("font.pln.sat", tMap, "arial", false, QFont::Normal, 14, MRGB(250, 250, 200));
  set->fonst[FONT_PLANET] = readFont("font.planet", tMap, "arial", false, QFont::Bold, 14, MRGB(250, 250, 250));
  set->fonst[FONT_HORIZON] = readFont("font.horizon", tMap, "arial", true, QFont::Bold, 16, MRGB(255, 255, 255));
  set->fonst[FONT_CONST] = readFont("font.const", tMap, "arial", true, QFont::Bold, 16, MRGB(225, 255, 225));
  set->fonst[FONT_COMET] = readFont("font.comet", tMap, "arial", false, QFont::Bold, 12, MRGB(250, 250, 250));
  set->fonst[FONT_ASTER] = readFont("font.aster", tMap, "arial", false, QFont::Bold, 12, MRGB(250, 250, 250));
  set->fonst[FONT_DRAWING] = readFont("font.drawing", tMap, "arial", false, QFont::Normal, 12, MRGB(250, 250, 250));
  set->fonst[FONT_TRACKING] = readFont("font.tracking", tMap, "arial", false, QFont::Normal, 12, MRGB(250, 250, 250));
  set->fonst[FONT_EARTH_SHD] = readFont("font.earth.shd", tMap, "arial", false, QFont::Bold, 12, MRGB(150, 255, 150));
  set->fonst[FONT_LUNAR_FEATURES] = readFont("font.lunar.features", tMap, "arial", false, QFont::Normal, 12, MRGB(255, 255, 0));
  set->fonst[FONT_GRID] = readFont("font.grid", tMap, "arial", false, QFont::Normal, 10, MRGB(150, 180, 150));
  set->fonst[FONT_SATELLITE] = readFont("font.satellite", tMap, "arial", false, QFont::Normal, 10, MRGB(150, 150, 150));
  set->fonst[FONT_SHOWER] = readFont("font.shower", tMap, "arial", false, QFont::Normal, 12, MRGB(220, 220, 220));
  set->fonst[FONT_STAR_VARS] = readFont("font.star.vars", tMap, "arial", false, QFont::Normal, 11, MRGB(220, 220, 220));

  // satellite
  set->map.satellite.color = readVal("map.satellite.color", MRGB(150, 160, 150), tMap).toUInt();
  set->map.satellite.size = readVal("map.satellite.size", 1, tMap).toDouble();

  // earth shadow
  set->map.es.show = readVal("map.es.show", true, tMap).toBool();
  set->map.es.color = readVal("map.es.color", MRGB(0, 200, 0), tMap).toUInt();
  set->map.es.alpha = readVal("map.es.alpha", 128, tMap).toFloat();

  // stars
  set->map.star.propNamesFromFov = readVal("map.star.propNamesFromFov", D2R(90), tMap).toDouble();
  set->map.star.bayerFromFov = readVal("map.star.bayerFromFov", D2R(50), tMap).toDouble();
  set->map.star.flamsFromFov = readVal("map.star.flamsFromFov", D2R(30), tMap).toDouble();
  set->map.star.bayerPriority = readVal("map.star.bayerPriority", true, tMap).toBool();
  set->map.star.namePriority = readVal("map.star.namePriority", false, tMap).toBool();
  set->map.star.useSpectralTp = readVal("map.star.useSpectralTp", true, tMap).toBool();
  set->map.star.starSizeFactor = readVal("map.star.starSizeFactor", -0.5, tMap).toDouble();
  set->map.star.saturation = readVal("map.star.saturation", 100, tMap).toDouble();
  set->map.star.properMotionYearVec = readVal("map.star.properMotionYearVec", 500, tMap).toDouble();
  set->map.star.showProperMotion = readVal("map.star.showProperMotion", false, tMap).toBool();
  set->map.star.useProperMotion = readVal("map.star.useProperMotion", true, tMap).toBool();
  set->map.star.showGlow = readVal("map.star.showGlow", false, tMap).toBool();
  set->map.star.glowAlpha = readVal("map.star.glowAlpha", 1.0, tMap).toFloat();
  set->map.star.varsFromFov = readVal("map.star.varsFromFov", D2R(25), tMap).toDouble();
  set->map.star.showVarLabels = readVal("map.star.showVarLabels", true, tMap).toBool();
  set->map.starBitmapName = readVal("map.starBitmapName", "../data/stars/bitmaps/stars2.png", tMap).toString();  


  // horizon
  set->map.hor.color = readVal("map.hor.color", MRGB(48, 68, 48), tMap).toUInt();
  set->map.hor.alpha = readVal("map.hor.alpha", 200, tMap).toInt();
  set->map.hor.cb_hor_show_alt_azm = readVal("map.hor.cb_hor_show_alt_azm", true, tMap).toBool();
  set->map.hor.showDirections = readVal("map.hor.showDirections", true, tMap).toBool();
  set->map.hor.hideTextureWhenMove = readVal("map.hor.hideTextureWhenMove", false, tMap).toBool();

  // dso
  set->map.dsoColors[DSO_COL_NEBULA] = readVal("map.dsoColor_nebula", MRGB(200, 200, 200), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_NEBULA] = readVal("map.dsoStyle_nebula", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_NEBULA] = readVal("map.dsoWidth_nebula", 1, tMap).toInt();

  set->map.dsoColors[DSO_COL_BRIGHT_NEB] = readVal("map.dsoColor_bright.neb", MRGB(200, 200, 200), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_BRIGHT_NEB] = readVal("map.dsoStyle_bright.neb", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_BRIGHT_NEB] = readVal("map.dsoWidth_bright.neb", 1, tMap).toInt();

  set->map.dsoColors[DSO_COL_DARK_NEB] = readVal("map.dsoColor_dark.neb", MRGB(100, 100, 100), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_DARK_NEB] = readVal("map.dsoStyle_dark.neb", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_DARK_NEB] = readVal("map.dsoWidth_dark.neb", 1, tMap).toInt();

  set->map.dsoColors[DSO_COL_OPEN_CLS] = readVal("map.dsoColor_open.cls", MRGB(190, 190, 32), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_OPEN_CLS] = readVal("map.dsoStyle_open.cls", (int)Qt::DotLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_OPEN_CLS] = readVal("map.dsoWidth_open.cls", 2, tMap).toInt();

  set->map.dsoColors[DSO_COL_GLOB_CLS] = readVal("map.dsoColor_glob.cls", MRGB(100, 150, 100), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_GLOB_CLS] = readVal("map.dsoStyle_glob.cls", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_GLOB_CLS] = readVal("map.dsoWidth_glob.cls", 2, tMap).toInt();

  set->map.dsoColors[DSO_COL_PLN_NEB] = readVal("map.dsoColor_pln.neb", MRGB(150, 200, 250), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_PLN_NEB] = readVal("map.dsoStyle_pln.neb", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_PLN_NEB] = readVal("map.dsoWidth_pln.neb", 2, tMap).toInt();

  set->map.dsoColors[DSO_COL_GALAXY] = readVal("map.dsoColor_galaxy", MRGB(150, 100, 100), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_GALAXY] = readVal("map.dsoStyle_galaxy", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_GALAXY] = readVal("map.dsoWidth_galaxy", 2, tMap).toInt();

  set->map.dsoColors[DSO_COL_GAL_CLS] = readVal("map.dsoColor_gal.cls", MRGB(120, 100, 100), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_GAL_CLS] = readVal("map.dsoStyle_gal.cls", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_GAL_CLS] = readVal("map.dsoWidth_gal.cls", 1, tMap).toInt();

  set->map.dsoColors[DSO_COL_STAR] = readVal("map.dsoColor_star", MRGB(120, 120, 120), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_STAR] = readVal("map.dsoStyle_star", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_STAR] = readVal("map.dsoWidth_star", 1, tMap).toInt();

  set->map.dsoColors[DSO_COL_OTHER] = readVal("map.dsoColor_other", MRGB(150, 150, 150), tMap).toUInt();
  set->map.dsoStyle[DSO_COL_OTHER] = readVal("map.dsoStyle_other", (int)Qt::SolidLine, tMap).toInt();
  set->map.dsoWidth[DSO_COL_OTHER] = readVal("map.dsoWidth_other", 1, tMap).toInt();

  set->map.dsoShapeColor[0] = readVal("map.dsoShapeColor0", MRGB(86, 86, 86), tMap).toUInt();
  set->map.dsoShapeColor[1] = readVal("map.dsoShapeColor1", MRGB(64, 64, 64), tMap).toUInt();
  set->map.dsoShapeColor[2] = readVal("map.dsoShapeColor2", MRGB(32, 32, 32), tMap).toUInt();
  set->map.dsoShapeAlpha = readVal("map.dsoShapeAlpha", 128, tMap).toUInt();

  set->map.dsoNoMagShapeFOV = readVal("map.dsoNoMagShapeFOV", D2R(75), tMap).toDouble();
  set->map.dsoNoMagOtherFOV = readVal("map.dsoNoMagOtherFOV", D2R(25), tMap).toDouble();

  set->map.dsoFilter = readVal("map.dsoFilter", "", tMap).toString();
  set->map.dsoFilterType = readVal("map.dsoFilterType", false, tMap).toBool();

  set->map.dsoFadeTo = readVal("map.dsoFadeTo", false, tMap).toBool();
  set->map.dsoNoMagFadeFOV = readVal("map.dsoNoMagFadeFOV", D2R(15), tMap).toDouble();

  for (int i = 0; i < DSOT_COUNT; i++)
  {
    set->map.dsoTypeShow[i] = readVal("map.dsoTypeShow" + QString::number(i), true, tMap).toBool();
    set->map.dsoTypeShowAll[i] = readVal("map.dsoTypeShowAll" + QString::number(i), true, tMap).toBool();
  }

  set->map.starRange[0].fromFov = readVal("map.starRangeFOV0", starMagnitudes[0][0], tMap).toDouble();
  set->map.starRange[0].mag = readVal("map.starRangeMag0", starMagnitudes[0][1], tMap).toDouble();
  set->map.starRange[1].fromFov = readVal("map.starRangeFOV1", starMagnitudes[1][0], tMap).toDouble();
  set->map.starRange[1].mag = readVal("map.starRangeMag1", starMagnitudes[1][1], tMap).toDouble();
  set->map.starRange[2].fromFov = readVal("map.starRangeFOV2", starMagnitudes[2][0], tMap).toDouble();
  set->map.starRange[2].mag = readVal("map.starRangeMag2", starMagnitudes[2][1], tMap).toDouble();
  set->map.starRange[3].fromFov = readVal("map.starRangeFOV3", starMagnitudes[3][0], tMap).toDouble();
  set->map.starRange[3].mag = readVal("map.starRangeMag3", starMagnitudes[3][1], tMap).toDouble();
  set->map.starRange[4].fromFov = readVal("map.starRangeFOV4", starMagnitudes[4][0], tMap).toDouble();
  set->map.starRange[4].mag = readVal("map.starRangeMag4", starMagnitudes[4][1], tMap).toDouble();
  set->map.starRange[5].fromFov = readVal("map.starRangeFOV5", starMagnitudes[5][0], tMap).toDouble();
  set->map.starRange[5].mag = readVal("map.starRangeMag5", starMagnitudes[5][1], tMap).toDouble();
  set->map.starRange[6].fromFov = readVal("map.starRangeFOV6", starMagnitudes[6][0], tMap).toDouble();
  set->map.starRange[6].mag = readVal("map.starRangeMag6", starMagnitudes[6][1], tMap).toDouble();
  set->map.starRange[7].fromFov = readVal("map.starRangeFOV7", starMagnitudes[7][0], tMap).toDouble();
  set->map.starRange[7].mag = readVal("map.starRangeMag7", starMagnitudes[7][1], tMap).toDouble();
  set->map.starRange[8].fromFov = readVal("map.starRangeFOV8", starMagnitudes[8][0], tMap).toDouble();
  set->map.starRange[8].mag = readVal("map.starRangeMag8", starMagnitudes[8][1], tMap).toDouble();
  set->map.starRange[9].fromFov = readVal("map.starRangeFOV9", starMagnitudes[9][0], tMap).toDouble();
  set->map.starRange[9].mag = readVal("map.starRangeMag9", starMagnitudes[9][1], tMap).toDouble();

  set->map.dsoRange[0].fromFov = readVal("map.dsoRangeFOV0", D2R(90), tMap).toDouble();
  set->map.dsoRange[0].mag = readVal("map.dsoRangeMag0", 5.5, tMap).toDouble();
  set->map.dsoRange[1].fromFov = readVal("map.dsoRangeFOV1", D2R(75), tMap).toDouble();
  set->map.dsoRange[1].mag = readVal("map.dsoRangeMag1", 6.0, tMap).toDouble();
  set->map.dsoRange[2].fromFov = readVal("map.dsoRangeFOV2", D2R(50), tMap).toDouble();
  set->map.dsoRange[2].mag = readVal("map.dsoRangeMag2", 7.0, tMap).toDouble();
  set->map.dsoRange[3].fromFov = readVal("map.dsoRangeFOV3", D2R(30), tMap).toDouble();
  set->map.dsoRange[3].mag = readVal("map.dsoRangeMag3", 8.0, tMap).toDouble();
  set->map.dsoRange[4].fromFov = readVal("map.dsoRangeFOV4", D2R(20), tMap).toDouble();
  set->map.dsoRange[4].mag = readVal("map.dsoRangeMag4", 9.0, tMap).toDouble();
  set->map.dsoRange[5].fromFov = readVal("map.dsoRangeFOV5", D2R(10), tMap).toDouble();
  set->map.dsoRange[5].mag = readVal("map.dsoRangeMag5", 10.0, tMap).toDouble();
  set->map.dsoRange[6].fromFov = readVal("map.dsoRangeFOV6", D2R(5), tMap).toDouble();
  set->map.dsoRange[6].mag = readVal("map.dsoRangeMag6", 11.3, tMap).toDouble();
  set->map.dsoRange[7].fromFov = readVal("map.dsoRangeFOV7", D2R(2.5), tMap).toDouble();
  set->map.dsoRange[7].mag = readVal("map.dsoRangeMag7", 12.5, tMap).toDouble();
  set->map.dsoRange[8].fromFov = readVal("map.dsoRangeFOV8", D2R(1), tMap).toDouble();
  set->map.dsoRange[8].mag = readVal("map.dsoRangeMag8", 15.0, tMap).toDouble();
  set->map.dsoRange[9].fromFov = readVal("map.dsoRangeFOV9", D2R(0.25), tMap).toDouble();
  set->map.dsoRange[9].mag = readVal("map.dsoRangeMag9", 20.0, tMap).toDouble();

  // grids
  set->map.grid[SMCT_RA_DEC].type = readVal("map.grid.ra.dec.type", GRID_ALL , tMap).toInt();
  set->map.grid[SMCT_RA_DEC].color = readVal("map.grid.ra.dec.color", MRGB(48, 56, 48), tMap).toUInt();

  set->map.grid[SMCT_ALT_AZM].type = readVal("map.grid.alt.azm.type", GRID_ALL , tMap).toInt();
  set->map.grid[SMCT_ALT_AZM].color = readVal("map.grid.alt.azm.color", MRGB(48, 56, 48), tMap).toUInt();

  set->map.grid[SMCT_ECL].type = readVal("map.grid.ecl.type", GRID_EQ , tMap).toInt();
  set->map.grid[SMCT_ECL].color = readVal("map.grid.ecl.color", MRGB(48, 56, 48), tMap).toUInt();

  set->map.grid[SMCT_GAL].type = readVal("map.grid.gal.type", GRID_NONE , tMap).toInt();
  set->map.grid[SMCT_GAL].color = readVal("map.grid.gal.color", MRGB(48, 56, 48), tMap).toUInt();

  set->map.autoGrid = readVal("map.grid.auto", true, tMap).toBool();
  set->map.showGridLabels = readVal("map.grid.show.labels", true, tMap).toBool();

  set->map.showMeridian = readVal("map.grid.show.showMeridian", false, tMap).toBool();
  set->map.meridianColor = readVal("map.grid.show.meridianColor", MRGB(128, 150, 128), tMap).toUInt();

  // background
  set->map.background.bStatic = readVal("map.background.bStatic", false, tMap).toBool();
  set->map.background.staticColor = readVal("map.background.staticColor", MRGB(0, 0, 0), tMap).toUInt();

  set->map.background.dynamicColor[0] = readVal("map.background.dynamicColor0", MRGB(22, 22, 22), tMap).toUInt();
  set->map.background.dynamicColor[1] = readVal("map.background.dynamicColor1", MRGB(62, 64, 80), tMap).toUInt();
  set->map.background.dynamicColor[2] = readVal("map.background.dynamicColor2", MRGB(64, 64, 120), tMap).toUInt();
  set->map.background.useAltAzmOnly = readVal("map.background.useAltAzmOnly", true, tMap).toBool();

  // constellation
  set->map.constellation.main.color = readVal("map.constellation.main.color", MRGB(100, 100, 150), tMap).toUInt();
  set->map.constellation.main.style = readVal("map.constellation.main.style", (int)Qt::SolidLine, tMap).toInt();
  set->map.constellation.main.width = readVal("map.constellation.main.width", 1, tMap).toInt();

  set->map.constellation.sec.color = readVal("map.constellation.sec.color", MRGB(100, 100, 150), tMap).toUInt();
  set->map.constellation.sec.style = readVal("map.constellation.sec.style", (int)Qt::DotLine, tMap).toInt();
  set->map.constellation.sec.width = readVal("map.constellation.sec.width", 1, tMap).toInt();

  set->map.constellation.bnd.color = readVal("map.constellation.bnd.color", MRGB(100, 100, 150), tMap).toUInt();
  set->map.constellation.bnd.style = readVal("map.constellation.bnd.style", (int)Qt::DotLine, tMap).toInt();
  set->map.constellation.bnd.width = readVal("map.constellation.bnd.width", 1, tMap).toInt();

  set->map.constellation.linesFile = readVal("map.constellation.linesFile", "../data/constellation/default.lin", tMap).toString();
  set->map.constellation.language = readVal("map.constellation.language", "", tMap).toString();
  set->map.constellation.longNames = readVal("map.constellation.longNames", false, tMap).toBool();
  set->map.constellation.showNames = readVal("map.constellation.showNames", true, tMap).toBool();

  // measure point
  set->map.measurePoint.color = readVal("map.measurePoint.color", MRGB(255, 255, 255), tMap).toUInt();
  set->map.measurePoint.style = readVal("map.measurePoint.style", (int)Qt::SolidLine, tMap).toInt();
  set->map.measurePoint.width = readVal("map.measurePoint.width", 3, tMap).toInt();

  // planets
  set->map.planet.penColor = readVal("map.planet.penColor", MRGB(255, 255, 255), tMap).toUInt();
  set->map.planet.brColor = readVal("map.planet.brColor", MRGB(200, 200, 255), tMap).toUInt();
  set->map.planet.satColor = readVal("map.planet.satColor", MRGB(220, 220, 255), tMap).toUInt();
  set->map.planet.satColorShd = readVal("map.planet.satColorShd", MRGB(100, 100, 100), tMap).toUInt();
  set->map.planet.lunarFeatures = readVal("map.planet.lunarFeatures", MRGB(255, 255, 255), tMap).toUInt();
  set->map.planet.phaseAlpha = readVal("map.planet.phaseAlpha", 200, tMap).toInt();
  set->map.planet.plnRad = readVal("map.planet.plnRad", 5, tMap).toInt();
  set->map.planet.satRad = readVal("map.planet.satRad", 3, tMap).toInt();
  set->map.planet.jupGRSLon = readVal("map.planet.jupGRSLon", 236.0, tMap).toDouble();
  set->map.planet.jupGRSYearDrift = readVal("map.planet.jupGRSYearDrift", 16.6, tMap).toDouble();
  set->map.planet.jupGRSDate = readVal("map.planet.jupGRSDate", 2457388.5, tMap).toDouble();
  set->map.planet.moonImage = readVal("map.planet.moonImage", "", tMap).toString();
  set->map.planet.useCustomMoonTexture = readVal("map.planet.useCustomMoonTexture", false, tMap).toBool();

  // shower
  set->map.shower.bShowAll = readVal("map.shower.bShowAll", true, tMap).toBool();
  set->map.shower.scale = readVal("map.shower.scale", 1.0, tMap).toDouble();
  set->map.shower.daysBeforeAfterDate = readVal("map.shower.daysBeforeAfterDate", 10.0, tMap).toDouble();
  set->map.shower.color = readVal("map.shower.color", MRGB(220, 220, 220), tMap).toUInt();
  set->map.shower.nonActiveColor = readVal("map.shower.nonActiveColor", MRGB(120, 120, 120), tMap).toUInt();

  // aster & comets  
  set->map.comet.real = readVal("map.comet.real", true, tMap).toBool();
  set->map.comet.color = readVal("map.comet.color", MRGB(255, 255, 255), tMap).toUInt();
  set->map.comet.radius = readVal("map.comet.radius", 5, tMap).toInt();
  set->map.comet.plusMag = readVal("map.comet.plusMag", 5, tMap).toFloat();
  set->map.comet.maxMag = readVal("map.comet.maxMag", 16, tMap).toFloat();

  set->map.aster.color = readVal("map.aster.color", MRGB(255, 255, 255), tMap).toUInt();
  set->map.aster.radius = readVal("map.aster.radius", 5, tMap).toInt();
  set->map.aster.plusMag = readVal("map.aster.plusMag", 5, tMap).toFloat();
  set->map.aster.maxMag = readVal("map.aster.magMag", 16, tMap).toFloat();

  // milky way
  set->map.milkyWay.bShow = readVal("map.milkyWay.bShow", true, tMap).toBool();
  set->map.milkyWay.light = readVal("map.milkyWay.light", 20, tMap).toInt();
  set->map.milkyWay.dark = readVal("map.milkyWay.dark", 15, tMap).toInt();
  set->map.milkyWay.darkest = readVal("map.milkyWay.darkest", 10, tMap).toInt();
  set->map.milkyWay.sameAsBkColor = readVal("map.milkyWay.sameAsBkColor", true, tMap).toBool();
  set->map.milkyWay.color = readVal("map.milkyWay.color", MRGB(30, 30, 30), tMap).toUInt();

  // drawing
  set->map.drawing.color = readVal("map.drawing.color", MRGB(255, 255, 255), tMap).toUInt();

  // object selection
  set->map.objSelectionColor = readVal("map.objSelectionColor", MRGB(255, 55, 55), tMap).toUInt();

  // tracking
  set->map.tracking.color = readVal("map.tracking.color", MRGB(225, 255, 225), tMap).toUInt();

  // USNO2
  set->map.usno2.show = readVal("map.usno2.show", false, tMap).toBool();
  set->map.usno2.fromFOV = readVal("map.usno2.fromFOV", D2R(5), tMap).toDouble();
  set->map.usno2.fromMag = readVal("map.usno2.fromMag", 13, tMap).toDouble();

  // URAT1
  set->map.urat1.show = readVal("map.urat1.show", false, tMap).toBool();
  set->map.urat1.fromFOV = readVal("map.urat1.fromFOV", D2R(6), tMap).toDouble();
  set->map.urat1.fromMag = readVal("map.urat1.fromMag", 13, tMap).toDouble();

  // USNO B1
  set->map.usnob1.show = readVal("map.usnob1.show", false, tMap).toBool();
  set->map.usnob1.fromFOV = readVal("map.usnob1.fromFOV", D2R(2), tMap).toDouble();
  set->map.usnob1.fromMag = readVal("map.usnob1.fromMag", 13, tMap).toDouble();

  // UCAC4
  set->map.ucac4.show = readVal("map.ucac4.show", false, tMap).toBool();
  set->map.ucac4.fromFOV = readVal("map.ucac4.fromFOV", D2R(15), tMap).toDouble();
  set->map.ucac4.fromMag = readVal("map.ucac4.fromMag", 12, tMap).toDouble();;

  // PPMXL
  set->map.ppmxl.show = readVal("map.ppmxl.show", false, tMap).toBool();
  set->map.ppmxl.fromFOV = readVal("map.ppmxl.fromFOV", D2R(5), tMap).toDouble();
  set->map.ppmxl.fromMag = readVal("map.ppmxl.fromMag", 13, tMap).toDouble();

  // NOMAD
  set->map.nomad.show = readVal("map.nomad.show", false, tMap).toBool();
  set->map.nomad.fromFOV = readVal("map.nomad.fromFOV", D2R(3), tMap).toDouble();
  set->map.nomad.fromMag = readVal("map.nomad.fromMag", 13, tMap).toDouble();

  // GSC
  set->map.gsc.show = readVal("map.gsc.show", false, tMap).toBool();
  set->map.gsc.fromFOV = readVal("map.gsc.fromFOV", D2R(10), tMap).toDouble();
  set->map.gsc.fromMag = readVal("map.gsc.fromMag", 11, tMap).toDouble();

  set->map.smartLabels = readVal("map.smartLabels", true, tMap).toBool();

  setCreateFonts();

  if (name.isEmpty() && !SkFile::exists(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/default.dat"))
  {
    setSave("default", set);
  }

  return true;
}

//////////////////////////////////////////
bool setSave2(QString name, setting_t *set)
//////////////////////////////////////////
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/" + name + ".dat");

  qDebug() << "saving profile" << f.fileName();

  if (f.open(SkFile::WriteOnly))
  {
    f.write((char *)set, sizeof(setting_t));
    f.close();
  }
  else
  {
    return(false);
  }

  return(true);
}

//////////////////////////////////////////
void setLoad2(QString name, setting_t *set)
//////////////////////////////////////////
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/" + name + ".dat");

  qDebug() << "load profile" << f.fileName();

  if (f.open(SkFile::ReadOnly))
  {
    if (f.size() == sizeof(setting_t))
    {
      f.read((char *)set, sizeof(setting_t));
    }
    else
    {
      f.read((char *)set, f.size());
    }
    f.close();
    setCreateFonts();
  }
}

//////////////////////////////
QFont setGetFonts(font_t *fnt)
//////////////////////////////
{
  QFont font;

  font.setPixelSize(fnt->size);
  font.setWeight(fnt->weight);
  font.setItalic(fnt->italic);
  font.setFamily(fnt->name);

  return(font);
}

////////////////////////////////////
void setSetFont(int no, CSkPainter *p)
////////////////////////////////////
{
  p->setFont(setFonts[no]);
}

/////////////////////////////////////////
void setSetFontColor(int no, CSkPainter *p)
/////////////////////////////////////////
{
  p->setPen(g_skSet.fonst[no].color);
}

/////////////////////////
void setCreateFonts(void)
/////////////////////////
{
  for (int i = 0; i < FONT_COUNT; i++)
  {
    setFonts[i].setPixelSize(g_skSet.fonst[i].size);
    setFonts[i].setWeight(g_skSet.fonst[i].weight);
    setFonts[i].setItalic(g_skSet.fonst[i].italic);
    setFonts[i].setFamily(g_skSet.fonst[i].name);
  }
}


////////////////////////////////////////////////////////////////////////////////////////////
void setSetFontItem(int no, const QString &name, bool italic, int weight, int size, QRgb color)
////////////////////////////////////////////////////////////////////////////////////////////
{
  g_skSet.fonst[no].italic = italic;
  g_skSet.fonst[no].weight = weight;
  g_skSet.fonst[no].size = size;
  g_skSet.fonst[no].color = color;
  g_skSet.fonst[no].name = name;
}

void setDefaultStarMagRanges(setting_t *set)
{
  set->map.starRange[0].fromFov = D2R(90);
  set->map.starRange[0].mag = 5.5;

  set->map.starRange[1].fromFov = D2R(75);
  set->map.starRange[1].mag = 6.0;

  set->map.starRange[2].fromFov = D2R(50);
  set->map.starRange[2].mag = 7.0;

  set->map.starRange[3].fromFov = D2R(30);
  set->map.starRange[3].mag = 8.0;

  set->map.starRange[4].fromFov = D2R(20);
  set->map.starRange[4].mag = 9.0;

  set->map.starRange[5].fromFov = D2R(10);
  set->map.starRange[5].mag = 10.0;

  set->map.starRange[6].fromFov = D2R(5);
  set->map.starRange[6].mag = 10.5;

  set->map.starRange[7].fromFov = D2R(2.5);
  set->map.starRange[7].mag = 12.5;

  set->map.starRange[8].fromFov = D2R(1);
  set->map.starRange[8].mag = 15;

  set->map.starRange[9].fromFov = D2R(0.25);
  set->map.starRange[9].mag = 20;
}

void restoreFromNightConfig(void)
{  
  g_pApp->setStyleSheet(qssStyle);
  g_nightConfig = false; 
}

void restoreFromPrintConfig(void)
{
  g_skSet = skPrintTmp;
  setCreateFonts();
  cStarRenderer.open(g_skSet.map.starBitmapName);
}

void setNightConfig(void)
{    
  g_nightConfig = true;
  qssStyle = g_pApp->styleSheet();
  g_pApp->setStyleSheet(readAllFile("../data/styles/night.qss"));  
}


void setPrintConfig(void)
{
  QRgb black = MRGB(255, 255, 255);
  QRgb dred = MRGB(0,0,0);
  QRgb lred = MRGB(128,128,128);
  QRgb cred = MRGB(64, 64, 64);
  QRgb lred2 = MRGB(100,100,100);
  QRgb vred = MRGB(200, 200, 200);

  skPrintTmp = g_skSet;

  for (int i = 0; i < FONT_COUNT; i++)
  {
    g_skSet.fonst[i].color = lred;
  }

  // earth shadow
  g_skSet.map.es.color = dred;

  g_skSet.map.starBitmapName = "../data/stars/bitmaps/print.png";
  g_skSet.map.star.starSizeFactor = 0;
  cStarRenderer.open(g_skSet.map.starBitmapName);

  // horizon
  g_skSet.map.hor.color = dred;

  // dso
  g_skSet.map.dsoColors[DSO_COL_NEBULA] = dred;
  g_skSet.map.dsoColors[DSO_COL_BRIGHT_NEB] = dred;;
  g_skSet.map.dsoColors[DSO_COL_DARK_NEB] = dred;
  g_skSet.map.dsoColors[DSO_COL_OPEN_CLS] = dred;;
  g_skSet.map.dsoColors[DSO_COL_GLOB_CLS] = dred;
  g_skSet.map.dsoColors[DSO_COL_PLN_NEB] = dred;
  g_skSet.map.dsoColors[DSO_COL_GALAXY] = dred;
  g_skSet.map.dsoColors[DSO_COL_GAL_CLS] = dred;
  g_skSet.map.dsoColors[DSO_COL_STAR] = dred;
  g_skSet.map.dsoColors[DSO_COL_OTHER] = dred;
  g_skSet.map.dsoShapeColor[0] = lred;
  g_skSet.map.dsoShapeColor[1] = lred2;
  g_skSet.map.dsoShapeColor[2] = cred;

  // grids
  g_skSet.map.grid[SMCT_RA_DEC].color = vred;
  g_skSet.map.grid[SMCT_ALT_AZM].color = vred;
  g_skSet.map.grid[SMCT_ECL].color = vred;
  g_skSet.map.grid[SMCT_GAL].color = vred;

  // background
  g_skSet.map.background.bStatic = true;
  g_skSet.map.background.staticColor = black;

  // constellation
  g_skSet.map.constellation.main.color = lred;
  g_skSet.map.constellation.sec.color = lred;
  g_skSet.map.constellation.bnd.color = lred;

  g_skSet.map.measurePoint.color = lred;

  // planets
  g_skSet.map.planet.penColor = lred;
  g_skSet.map.planet.brColor = lred2;
  g_skSet.map.planet.satColor = lred;
  g_skSet.map.planet.satColorShd = dred;
  g_skSet.map.planet.lunarFeatures = lred;
  g_skSet.map.planet.phaseAlpha = 128;

  // comet & aster.
  g_skSet.map.comet.color = lred;
  g_skSet.map.aster.color = lred;

  // milky way
  g_skSet.map.milkyWay.bShow = true;
  g_skSet.map.milkyWay.light = -20;
  g_skSet.map.milkyWay.dark = -10;

  // drawing
  g_skSet.map.drawing.color = lred;

  // object selection
  g_skSet.map.objSelectionColor = lred;

  // tracking
  g_skSet.map.tracking.color = lred;
}

///////////////////////////
void setSetDefaultVal(void)
///////////////////////////
{
  setLoad("");
}
