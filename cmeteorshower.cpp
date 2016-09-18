#include "cmeteorshower.h"
#include "transform.h"
#include "smartlabeling.h"
#include "setting.h"
#include "mapobj.h"

CMeteorShower::CMeteorShower() :
  m_year(-1)
{
}

void CMeteorShower::render(CSkPainter *p, mapView_t *view)
{
  double scale = 1.25;
  bool showAll = true;//false;
  double beforeAfterDate = 5;

  p->setBrush(Qt::NoBrush);

  int i = 0;
  for (const CMeteorShowerItem &item : m_list)
  {
    SKPOINT pt;

    if (showAll || ((view->jd >= item.jdBegin - beforeAfterDate) && (view->jd <= item.jdEnd + beforeAfterDate)))
    {
      trfRaDecToPointNoCorrect(&item.rd, &pt);

      if (trfProjectPoint(&pt))
      {
        p->setPen(QPen(Qt::white, 3));
        p->drawCircle(QPoint(pt.sx, pt.sy), 10 * scale);

        g_labeling.addLabel(QPoint(pt.sx, pt.sy), 15 * scale, item.name, FONT_SHOWER, RT_BOTTOM_RIGHT, SL_AL_ALL);
        addMapObj(item.rd, pt.sx, pt.sy, MO_SHOWER, MO_CIRCLE, 12 * scale, (qint64)&m_list[i], 0);

        p->setPen(QPen(Qt::white, 1, Qt::DotLine));

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

void CMeteorShower::load(int year)
{
  m_list.clear();

  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QString("/data/catalogue/showers/%1.dat").arg(year));

  if (!f.open(QFile::ReadOnly | QFile::Text))
  {
    m_year = -1;
    return;
  }

  m_year = year;

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

    if (list.count() != 9)
    {
      continue;
    }

    CMeteorShowerItem item;

    item.name = list[0].simplified();

    QStringList rhm = list[1].split(":");

    double ra = 0;
    double dec = D2R(list[2].simplified().toDouble());
    int    months[3];
    int    days[3];
    int    years[3] = {year, year, year};

    if (rhm.count() == 2)
    {
      ra = HMS2RAD(rhm[0].toInt(), rhm[1].toInt(), 0);
    }

    QStringList jdl = list[3].split(".");
    if (jdl.length() == 2)
    {
      days[0] = jdl[0].toInt();
      months[0] = jdl[1].toInt();
    }

    jdl = list[4].split(".");
    if (jdl.length() == 2)
    {
      days[1] = jdl[0].toInt();
      months[1] = jdl[1].toInt();
    }

    jdl = list[5].split(".");
    if (jdl.length() == 2)
    {
      days[2] = jdl[0].toInt();
      months[2] = jdl[1].toInt();
    }

    if (months[0] > months[1]) years[0]--;
    if (months[2] < months[1]) years[2]++;

    item.jdBegin = jdGetJDFrom_DateTime(years[0], months[0], days[0]);
    item.jdMax = jdGetJDFrom_DateTime(years[1], months[1], days[1]);
    item.jdEnd = jdGetJDFrom_DateTime(years[2], months[2], days[2]);

    item.rd.Ra = ra;
    item.rd.Dec = dec;

    precess(&item.rd, &item.rd, item.jdMax, JD2000);

    item.name = list[0].simplified();
    item.rate = list[6].simplified();
    item.speed = list[7].simplified().toDouble();
    item.source = list[8].simplified();

    m_list.append(item);
  }
}


