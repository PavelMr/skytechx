#include <QtGui>

#include <QtCore>
#include <QAudioDeviceInfo>
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
#include "soundmanager.h"
#include "hipsrenderer.h"

static QString LOG_FILE;

int g_ocTreeDepth = 4;
bool g_developMode = false;
bool g_showFps = false;
bool g_log = false;

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

void messageHandler(QtMsgType type, const QMessageLogContext & /*context*/, const QString &msg)
{
  QString txt;
  switch (type)
  {
    case QtDebugMsg:
      txt = QString("Debug: %1").arg(msg);
      break;

    case QtInfoMsg:
      txt = QString("Info: %1").arg(msg);
      break;

    case QtWarningMsg:
      txt = QString("Warning: %1").arg(msg);
      break;

    case QtCriticalMsg:
      txt = QString("Critical: %1").arg(msg);
      break;

    case QtFatalMsg:
      txt = QString("Fatal: %1").arg(msg);
      abort();
  }

  QFile outFile(LOG_FILE);
  outFile.open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream ts(&outFile);
  ts << txt << endl;
  ts.flush();
}

static void copyPath(const QString &src, const QString &dst)
{
  QDir dir(src);
  if (!dir.exists())
  {
    return;
  }

  foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
  {
    QString dst_path = dst + QDir::separator() + d;
    dir.mkpath(dst_path);
    copyPath(src + QDir::separator() + d, dst_path);
  }

  foreach (QString f, dir.entryList(QDir::Files))
  {
    QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
  }
}

void copyAppData()
{
  // copy app data to current user folder (if not exists)
  copyPath("../appdata", QStandardPaths::writableLocation(QStandardPaths::DataLocation));
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
  QCoreApplication::setApplicationName("SkytechX_beta"); // NOTE: beta reg.
  QCoreApplication::setApplicationVersion(SK_VERSION);

  QApplication a(argc, argv);
  
  QSharedMemory sharedMemory("The SkytechX (PMR)");
  if (!sharedMemory.create(1) && sharedMemory.error() == QSharedMemory::AlreadyExists)
  {
    msgBoxError(NULL, QObject::tr("SkytechX already running!"));
    return 1;
  }  

  LOG_FILE = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/log/log.txt";
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/log");   

  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables");

  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/locations");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/timemarks");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/tracking");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/events");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/drawing");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/comets");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/asteroids");
  checkAndCreateFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/showers");
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

  copyAppData();

  g_soundManager.init();

  g_systemSettings = new SystemSettings("../skytechx.cfg");
  g_systemSettings->readAll();

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
    else
    if (param.startsWith("-log"))
    {
      g_log = true;
    }
  }  



  if (g_log)
  {
    QFile::remove(LOG_FILE);
    qInstallMessageHandler(messageHandler);
  } 

  QString languagePath = settings.value("language").toString();

  QTranslator translator;  

  if (!languagePath.isEmpty())
  {
    translator.load(languagePath);
    a.installTranslator(&translator);
  }
  else
  {
    QLocale loc;
    QSettings set;
    switch (loc.language())
    {
      case QLocale::Czech:
        languagePath = "../languages/cesky.qm";
        set.setValue("language", languagePath);
        break;
    }

    if (!languagePath.isEmpty())
    {
      if (translator.load(languagePath))
      {
        a.installTranslator(&translator);
      }
    }
  }  

  QLocale::setDefault(QLocale::c());
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  qDebug("octreedepth=%d", g_ocTreeDepth);
  qDebug("numthreads=%d", omp_get_max_threads());
  qDebug("develop=%d", g_developMode);
  qDebug("SSL support %d", QSslSocket::supportsSsl());
  qDebug() << "SSL ver" << QSslSocket::sslLibraryVersionString();
  qDebug() << "SSL ver" << QSslSocket::sslLibraryBuildVersionString();
  qDebug("Sound support %d", !QAudioDeviceInfo::availableDevices(QAudio::AudioOutput).isEmpty());

  g_hipsRenderer = new HiPSRenderer();

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

  return ret;
}
