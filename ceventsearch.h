/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CEVENTSEARCH_H
#define CEVENTSEARCH_H

#include <QtGui>
#include <QtCore>

#include "cmapview.h"
#include "ceventprogdlg.h"
#include "castro.h"

#define EVT_ELONGATION         0
#define EVT_OPPOSITION         1
#define EVT_CONJUCTION         2
#define EVT_SOLARECL           3
#define EVT_SUNTRANSIT         4
#define EVT_OCCULTATION        5
#define EVT_LUNARECL           6
#define EVT_BIG_MOON           7

#define EVV_NONE               0
#define EVV_PARTIAL            1
#define EVV_FULL               2

#define EVE_FULL               0
#define EVE_RING               1
#define EVE_PARTIAL            2

#define EVLE_PARTIAL_PENUMBRA  0
#define EVLE_FULL_PENUMBRA     1
#define EVLE_PARTIAL_UMBRA     2
#define EVLE_FULL_UMBRA        3

#define EVENT_HEADER_ID        0xE0000003

typedef struct
{
  int     type;      // EVT_cxx
  int     vis;       // EVV_cxx
  double  jd;        // jd at center of event
  int     id;        // object id
  quint64 geoHash;
  QString locationName;
  bool    geocentric;

  union
  {
    struct
    {
      double elong;
    } elongation_u;

    struct
    {
      double size;
      double R;
    } opposition_u;

    struct
    {
      double dist;
      int    idList[PT_PLANET_COUNT];
      int    idCount;
    } conjuction_u;

    struct
    {
      double c1, c2; // first & last outer contact
      double i1, i2; // first & last inner contact (or -1)
      int    id;
    } sunTransit_u;

    struct
    {
      double c1, c2; // first & last outer contact
      double i1, i2; // first & last inner contact (or -1)
      int    id;
    } moonOcc_u;

    struct
    {
      double p1, p2;
      double p3, p4;
      double u1, u2;
      double u3, u4;
      int    type;   // EVLE_xxx
      double mag;    // eclipse magnitude
    } lunarEcl_u;

    struct
    {
      double c1, c2; // first & last outer contact
      double i1, i2; // first & last inner contact (or -1)
      int    type;   // EVE_xxx
      double mag;    // eclipse magnitude
    } solarEcl_u;

    struct
    {
      double R;
    } bigMoon_u;
  } event_u;

} event_t;

extern QList <event_t *> tEventList;
extern int               nLastEventCount;

namespace searchEvent
{
  class CEventThread : public QThread
  {
    Q_OBJECT

    public:
      bool m_end;
  };

  class CMaxElongation : public CEventThread
  {
    Q_OBJECT

  public:
       void run(void);
     double m_jdFrom;
     double m_jdTo;
        int m_id;
       bool m_highPrec;
  mapView_t m_view;

  signals:
    void sigDone(void);
    void sigProgress(int per, int id, int founded);
  };


  class COpposition : public CEventThread
  {
    Q_OBJECT

  public:
       void run(void);
     double m_jdFrom;
     double m_jdTo;
        int m_id;
       bool m_highPrec;
  mapView_t m_view;

  signals:
    void sigDone(void);
    void sigProgress(int per, int id, int founded);
  };

  class CBigMoon : public CEventThread
  {
    Q_OBJECT

  public:
       void run(void);
     double m_jdFrom;
     double m_jdTo;
       bool m_highPrec;
  mapView_t m_view;

  signals:
    void sigDone(void);
    void sigProgress(int per, int id, int founded);
  };

  class CConjuction : public CEventThread
  {
    Q_OBJECT

  public:
       void run(void);
     double m_jdFrom;
     double m_jdTo;
      QList <int>m_idList;
       bool m_highPrec;
     double m_maxDist;
  mapView_t m_view;

  signals:
    void sigDone(void);
    void sigProgress(int per, int id, int founded);
  };

  class CCommonEvent : public CEventThread
  { // eclipse, occultation, transit
    Q_OBJECT

  public:
       void run(void);
       void findFirstLastContact(double from, double *jdF, double *jdL, double *jdI1, double *jdI2, bool inner = true);
     double m_jdFrom;
     double m_jdTo;
        int m_id0;
        int m_id1;
        int m_type; // EVT_xxx
       bool m_highPrec;
     double m_maxDist;
  mapView_t m_view;
    event_t m_event;

  signals:
    void sigDone(void);
    void sigProgress(int per, int id, int founded);
  };

}

namespace Ui {
    class CEventSearch;
}

class CEventSearch : public QDialog
{
    Q_OBJECT

public:
    explicit CEventSearch(QWidget *parent, mapView_t *view);
    ~CEventSearch();

signals:
   void sigCancel(void);

protected:
    void changeEvent(QEvent *e);
    void setObjects(void);

    bool biggestMoon(void);
    bool maxElongation(void);
    bool opposition(void);
    bool conjuction(void);
    bool commonEvent(int type);

    double    jdFrom;
    double    jdTo;
    mapView_t m_view;

    QList <searchEvent::CEventThread *> tThread;

private slots:
    void on_radioButton_7_clicked();

    void on_radioButton_6_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_4_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_radioButton_8_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::CEventSearch *ui;
};

quint64 getGeoHash(mapView_t *view);

#endif // CEVENTSEARCH_H
