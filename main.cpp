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
#include "clog.h"
#include "systemsettings.h"

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
  QString str = readAllFile("../data/styles/night.qss");

  return(str);
}

/////////////////////////////
QString loadNormalStyle(void)
/////////////////////////////
{
  QString str = readAllFile("../data/styles/normal.qss");

  qDebug() << str;

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

#include "nutation.h"

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
  QCoreApplication::setApplicationName("SkytechX_104"); // FIXME:
  QCoreApplication::setApplicationVersion(SK_VERSION);

  QApplication a(argc, argv);

  QSharedMemory sharedMemory("The SkytechX (PMR)");
  if (!sharedMemory.create(1) && sharedMemory.error() == QSharedMemory::AlreadyExists)
  {
    msgBoxError(NULL, QObject::tr("SkytechX already running!"));
    return 1;
  }

  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/locations");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/timemarks");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/tracking");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/events");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/drawing");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/comets");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/asteroids");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/tle");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/default");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/sun");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/notes");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/db");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/horizons");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/gamepad");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dso");

  g_systemSettings = new SystemSettings("../skytechx.cfg");
  g_systemSettings->readAll();



  ///////////
  /// \brief settings
  ///////////

  /*
  int size = 10000000;

  int *val = (int *)malloc(4 * size);

  int *ia = (int *)malloc(4 * size);
  QList <int> il;
  QTime tsi;
  QTime tsl;

  for (int i = 0; i < size; i++)
  {
    val[i] = rand();
  }

  tsi.start();
  for (int i = 0; i < size; i++)
  {
    ia[i] = val[i];
  }
  qDebug() << tsi.elapsed();

  tsl.start();
  for (int i = 0; i < size; i++)
  {
    il.append(val[i]);
  }
  qDebug() << tsl.elapsed();

  ///////

  tsi.start();
  for (int i = 0; i < size; i++)
  {
    val[i] = ia[i];
  }
  qDebug() << tsi.elapsed();

  tsl.start();
  for (int i = 0; i < size; i++)
  {
    val[i] = il[i];
  }
  qDebug() << tsl.elapsed();

  */



  ///


  QSettings settings;

  g_pApp = &a;

  // parse cmd line
  for (int i = 1; i < argc; i++)
  {
    QString value;
    QString param = argv[i];

    if (getCommandParamValue(param, "-octreedepth=", "=", value))
    {
      g_ocTreeDepth = value.toInt();
    }
    else
    if (getCommandParamValue(param, "-numthreads=", "=", value))
    {
      omp_set_num_threads(value.toInt());
    }
    else
    if (getCommandParamValue(param, "-develop_mode=", "=", value))
    {
      g_developMode = value.toInt();
    }
    else
    if (getCommandParamValue(param, "-show_fps", "=", value))
    {
      g_showFps = value.toInt();
    }
    else
    if (param.startsWith("-reset_profile"))
    {
      settings.remove("");
    }
  }

  QString languagePath = settings.value("language").toString();

  QTranslator translator;

  if (!languagePath.isEmpty())
  {
    translator.load(languagePath);
    a.installTranslator(&translator);
  }

  QLocale::setDefault(QLocale::c());
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  qDebug("octreedepth=%d", g_ocTreeDepth);
  qDebug("numthreads=%d", omp_get_max_threads());
  qDebug("develop=%d", g_developMode);

  CLoadingDlg *dlg = new CLoadingDlg;
  dlg->exec();
  delete dlg;

  loadQSSStyle();

  MainWindow w;
  w.show();

  int ret = 1;

  try
  {
    ret = a.exec();
  }

  catch (QException &exc)
  {
    msgBoxError(NULL, exc.what());
    qDebug() << "ERROR" << exc.what();
  }

  catch (...)
  {
    qDebug() << "ERROR2";
  }

  /*
  catch (std::bad_alloc &exc)
  {
    msgBoxError(NULL, exc.what());
    qDebug() << "ERROR" << exc.what();
  }
  */

  return ret;
}
