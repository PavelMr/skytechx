#include "cmeteorshower.h"
#include "transform.h"
#include "smartlabeling.h"
#include "setting.h"
#include "mapobj.h"
#include "mainwindow.h"

extern MainWindow *pcMainWnd;

CMeteorShower g_meteorShower;

CMeteorShower::CMeteorShower() :
  m_year(-1)
{
}


QStringList CMeteorShower::getNameList()
{
  QStringList list;

  for (const CMeteorShowerItem &item : m_list)
  {
    list << item.name;
    list.append(item.name);
    list += item.name;
  }

  return list;
}

const CMeteorShowerItem *CMeteorShower::search(const QString &text) const
{
  for (const CMeteorShowerItem &item : m_list)
  {
    if (item.name.compare(text, Qt::CaseInsensitive) == 0)
    {
      return &item;
    }
  }

  return nullptr;
}


void CMeteorShower::render(CSkPainter *p, mapView_t *view)
{  
  double scale = g_skSet.map.shower.scale;
  bool showAll = g_skSet.map.shower.bShowAll;
  double beforeAfterDate = g_skSet.map.shower.daysBeforeAfterDate;

  p->setBrush(Qt::NoBrush);

  int i = 0;
  for (const CMeteorShowerItem &item : m_list)
  {
    SKPOINT pt;
    bool show = (view->jd >= item.jdBegin - beforeAfterDate) && (view->jd <= item.jdEnd + beforeAfterDate);
    bool isActive = (view->jd >= item.jdBegin) && (view->jd <= item.jdEnd);

    if (showAll || show)
    {
      trfRaDecToPointNoCorrect(&item.rd, &pt);            

      if (trfProjectPoint(&pt))
      {
        QColor color;

        if (isActive)
        {
          color = g_skSet.map.shower.color;
        }
        else
        {
          color = g_skSet.map.shower.nonActiveColor;
        }

        p->setPen(QPen(color, 3));
        p->drawCircle(QPoint(pt.sx, pt.sy), 10 * scale);

        g_labeling.addLabel(QPoint(pt.sx, pt.sy), 15 * scale, item.name, FONT_SHOWER, RT_BOTTOM_RIGHT, SL_AL_ALL);
        addMapObj(item.rd, pt.sx, pt.sy, MO_SHOWER, MO_CIRCLE, 12 * scale, (qint64)&m_list[i], 0);

        p->setPen(QPen(color, 1, Qt::DotLine));

        srand(12345);
        for (double a = 0; a < R360; a += D2R(45))
        {
          int min = (15 + (rand() % 15)) * scale;
          int max = (50 + (rand() % 55)) * scale ;

          double x = sin(a);
          double y = cos(a);

          p->drawLine(pt.sx + x * min, pt.sy + y * min,
                      pt.sx + x * max, pt.sy + y * max);
        }
      }
    }
    i++;
  }
}

static int toMonth(const QString &text)
{
  QString months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

   for (int i = 0; i < 12; i++)
   {
     if (text.simplified().compare(months[i], Qt::CaseInsensitive) == 0)
     {
       return i + 1;
     }
   }

   return 0;
}

void CMeteorShower::load(int year)
{
  if (m_year == year)
  { // already loaded
    return;
  }

  m_list.clear();

  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QString("/data/catalogue/showers/%1.dat").arg(year));

  if (!f.open(QFile::ReadOnly | QFile::Text))
  {
    pcMainWnd->removeQuickInfo(MO_SHOWER);
    m_year = -1;
    return;
  }

  m_year = year;

  pcMainWnd->removeQuickInfo(MO_SHOWER);

  qDebug() << "loading shower" << year;

  while (true)
  {
    QString str;
    QStringList list;

    str = f.readLine();
    if (str.isEmpty())
      break;

    if (str.simplified().startsWith("#"))
      continue;

    list = str.split('|');

    if (list.count() != 10)
    {
      continue;
    }

    CMeteorShowerItem item;    

    QStringList rhm = list[2].split(":");

    double ra = 0;
    double dec = D2R(list[3].simplified().toDouble());
    int    months[3];
    int    days[3];
    int    years[3] = {year, year, year};

    if (rhm.count() == 2)
    {
      ra = HMS2RAD(rhm[0].toInt(), rhm[1].toInt(), 0);
    }
    else
    {
      if (list[2].simplified().endsWith("d"))
      {
        QString str = list[2].simplified();
        str.chop(1);
        ra = D2R(str.toDouble());
      }
      else
      {
        continue;
      }
    }

    bool ok;
    QStringList jdl = list[4].split(".");
    if (jdl.length() == 2)
    {
      days[0] = jdl[0].toInt();
      months[0] = jdl[1].toInt(&ok);
      if (!ok)
      {
        months[0] = toMonth(jdl[1]);
      }
    }

    jdl = list[5].split(".");
    if (jdl.length() == 2)
    {
      days[1] = jdl[0].toInt();
      months[1] = jdl[1].toInt(&ok);
      if (!ok)
      {
        months[1] = toMonth(jdl[1]);
      }
    }

    jdl = list[6].split(".");
    if (jdl.length() == 2)
    {
      days[2] = jdl[0].toInt();
      months[2] = jdl[1].toInt(&ok);
      if (!ok)
      {
        months[2] = toMonth(jdl[1]);
      }
    }

    if (months[0] > months[1]) years[0]--;
    if (months[2] < months[1]) years[2]++;

    item.jdBegin = jdGetJDFrom_DateTime(years[0], months[0], days[0]);
    item.jdMax = jdGetJDFrom_DateTime(years[1], months[1], days[1]);
    item.jdEnd = jdGetJDFrom_DateTime(years[2], months[2], days[2]);

    item.rd.Ra = ra;
    item.rd.Dec = dec;    

    item.name = list[0].simplified();
    item.rate = list[7].simplified();
    item.speed = list[8].simplified().toDouble();
    item.source = list[9].simplified();
    item.epoch = list[1].simplified().toDouble();

    if (item.epoch > 0)
      precess(&item.rd, &item.rd, jdGetJDFromYear(item.epoch), JD2000);
    else
      precess(&item.rd, &item.rd, item.jdMax, JD2000);

    m_list.append(item);
  }
}


