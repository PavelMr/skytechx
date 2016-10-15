#ifndef SMARTLABELING_H
#define SMARTLABELING_H

#include <QPoint>
#include <QList>
#include <QRect>

class CSkPainter;

#define RT_CENTER         0
#define RT_TOP            1
#define RT_TOP_RIGHT      2
#define RT_RIGHT          3
#define RT_BOTTOM_RIGHT   4
#define RT_BOTTOM         5
#define RT_BOTTOM_LEFT    6
#define RT_LEFT           7
#define RT_TOP_LEFT       8

#define SL_AL_FIXED          0
#define SL_AL_CENTER         1
#define SL_AL_BOTTOM_LEFT    2
#define SL_AL_TOP_LEFT       3
#define SL_AL_BOTTOM_RIGHT   4
#define SL_AL_BOTTOM         5
#define SL_AL_TOP_RIGHT      6
#define SL_AL_ALL           10

class SmartLabel
{
public:
  QPoint  m_point;
  int     m_distance;
  int     m_defaultAlign;
  int     m_allowedAlign;
  QString m_text;
  int     m_fontId;

  QRect   m_rect;
  bool    m_done;
  double  m_opacity;
};

class SmartLabeling
{
public:
  SmartLabeling();
  void clear();
  void addLabel(const QPoint &point, int distance, const QString &text, int fontId, int defaultAlign, int allowedAlign, double opacity = 1.0);
  QRect renderLabel(CSkPainter *painter, const QPoint &point, float offset, const QString &text, int fontId, int align, bool render = true, double opacity = 1.0);
  void render(CSkPainter *painter);

private:
  QList <SmartLabel> m_list;
  QList <QRect>      doneList;

  static bool sortFnc(const SmartLabel &a, const SmartLabel &b)
  {
    return a.m_allowedAlign < b.m_allowedAlign;
  }
  int getAlign(int align);
};

extern SmartLabeling g_labeling;

#endif // SMARTLABELING_H
