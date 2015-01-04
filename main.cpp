#include <QtGui>

#include <QtCore>
#include <omp.h>
#include "skcore.h"
#include "build.h"

#include "mainwindow.h"
#include "cloadingdlg.h"
#include "cstylesel.h"
#include "Usno2A.h"
#include "cconsole.h"

// TODO: list
/*
 *
 *
 *
 * */

int g_ocTreeDepth = 4;
bool g_developMode = false;
bool g_showFps = false;

QApplication *g_pApp = NULL;

////////////////////////////
QString loadNightStyle(void)
////////////////////////////
{
  QString str = readAllFile("data/styles/night.qss");

  return(str);
}

/////////////////////////////
QString loadNormalStyle(void)
/////////////////////////////
{
  QString str = readAllFile("data/styles/normal.qss");

  return(str);
}

static bool getCommandParamValue(const QString command, const QString& param, const QString& separator, QString& value)
{
  if (!command.startsWith(param))
  {
    return false;
  }

  QStringList list = command.split(separator);

  if (list.count() != 2)
  {
    value = QString();
    return true;
  }

  value = list[1];

  return true;
}

////////////////////////////////
int main(int argc, char *argv[])
////////////////////////////////
{
  // Enable run-time memory check for debug builds.
   #if defined(DEBUG) | defined(_DEBUG)
     //_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
   #endif

  QCoreApplication::addLibraryPath("plugins");

  QCoreApplication::setOrganizationDomain("Frostware");
  QCoreApplication::setOrganizationName("PMR");
  QCoreApplication::setApplicationName("SkytechX");
  QCoreApplication::setApplicationVersion(SK_VERSION);

  qDebug() << "start";

#if 0
  Observer obs( 50.087, 14.42024, 0.5);
     Tle tle = Tle("UK-DMC 2                ",
         "1 25544U 98067A   15002.15562623  .00006329  00000-0  10729-3 0  6661",
         "2 25544  51.6455 192.6725 0006692 199.8843 268.0429 15.52906336922201");

     SGP4 sgp4(tle);

     DateTime now = DateTime(2015, 1, 2, 16, 23, 7);
             /*
              * calculate satellite position
              */
             Eci eci = sgp4.FindPosition(now);
             /*
              * get look angle for observer to satellite
              */
             CoordTopocentric topo = obs.GetLookAngle(eci);
             /*
              * convert satellite position to geodetic coordinates
              */
             CoordGeodetic geo = eci.ToGeodetic();

             qDebug() << now.ToJulian() << " " << R2D(geo.latitude) << R2D(geo.longitude) << geo.altitude;// << " " << geo;
             qDebug() << now.ToJulian() << " " << R2D(topo.azimuth) << R2D(topo.elevation) << topo.range;// << " " << geo;
#endif


  QApplication a(argc, argv);
  CLoadingDlg *dlg = new CLoadingDlg;

  QSettings settings;
  QString languagePath = settings.value("language").toString();

  QTranslator translator;

  if (!languagePath.isEmpty())
  {
    translator.load(languagePath);
    a.installTranslator(&translator);
  }

  g_pApp = &a;

  QLocale::setDefault(QLocale::c());
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
  // TODO: locale for QDateTimeEdit or custom dialog
  //QLocale::setDefault(QLocale::Afar);

  //pcDebug = new CConsole();

  // parse cmd line
  for (int i = 1; i < argc; i++)
  {
    QString value;
    if (getCommandParamValue(argv[i], "octreedepth=", "=", value))
    {
      g_ocTreeDepth = value.toInt();
    }
    else
    if (getCommandParamValue(argv[i], "numthreads=", "=", value))
    {
      omp_set_num_threads(value.toInt());
    }
    else
    if (getCommandParamValue(argv[i], "develop_mode=", "=", value))
    {
      g_developMode = value.toInt();
    }
    else
    if (getCommandParamValue(argv[i], "show_fps", "=", value))
    {
      g_showFps = value.toInt();
    }
  }

  qDebug("octreedepth=%d", g_ocTreeDepth);
  qDebug("numthreads=%d", omp_get_max_threads());

  //pcDebug->write("%s", qPrintable(a.libraryPaths().at(0)));
  //pcDebug->write("%s", qPrintable(a.libraryPaths().at(1)));


  loadQSSStyle();

  dlg->exec();
  delete dlg;

  MainWindow w;
  w.show();

  int ret = a.exec();

  return ret;
}
