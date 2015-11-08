#include "smartlabeling.h"
#include "cskpainter.h"
#include "setting.h"

SmartLabeling g_labeling;

SmartLabeling::SmartLabeling()
{
}

void SmartLabeling::clear()
{
  m_list.clear();
}

void SmartLabeling::addLabel(const QPoint &point, int distance, const QString &text, int fontId, int defaultAlign, int allowedAlign)
{
  SmartLabel label;

  label.m_point = point;
  label.m_distance = distance;
  label.m_text = text;
  label.m_fontId = fontId;
  label.m_defaultAlign = defaultAlign;
  label.m_allowedAlign = allowedAlign;

  m_list.append(label);
}


QRect SmartLabeling::renderLabel(CSkPainter *painter, const QPoint &point, float offset, const QString &text, int fontId, int align, bool render)
{
  QFont fnt = setFonts[fontId];
  QFontMetrics fm(fnt);
  QRect trc = fm.boundingRect(text);
  int x = point.x();
  int y = point.y();

  trc.moveLeft(x);
  trc.moveBottom(y);

  switch (align)
  {
    case RT_CENTER:
    {
      trc.moveCenter(QPoint(x, y));
      break;
    }
    case RT_TOP:
    {
      trc.moveLeft(x - trc.width() / 2);
      trc.moveBottom(y - offset);
      break;
    }
    case RT_TOP_RIGHT:
    {
      int offset2 = 0.5 * sqrt(POW2(offset) + POW2(offset));
      trc.moveLeft(x + offset2);
      trc.moveBottom(y - offset2);
      break;
    }
    case RT_TOP_LEFT:
    {
      int offset2 = 0.5 * sqrt(POW2(offset) + POW2(offset));
      trc.moveRight(x - offset2);
      trc.moveBottom(y - offset2);
      break;
    }
    case RT_BOTTOM_RIGHT:
    {
      int offset2 = 0.5 * sqrt(POW2(offset) + POW2(offset));
      trc.moveLeft(x + offset2);
      trc.moveTop(y + offset2);
      break;
    }
    case RT_BOTTOM_LEFT:
    {
      int offset2 = 0.5 * sqrt(POW2(offset) + POW2(offset));
      trc.moveRight(x - offset2);
      trc.moveTop(y + offset2);
      break;
    }
    case RT_BOTTOM:
    {
      trc.moveLeft(x - trc.width() / 2);
      trc.moveTop(y + offset);
      break;
    }
    case RT_RIGHT:
    {
      trc.moveLeft(x + offset);
      trc.moveTop(y - trc.height() / 2);
      break;
    }
    case RT_LEFT:
    {
      trc.moveRight(x - offset);
      trc.moveTop(y - trc.height() / 2);
      break;
    }
  }
  trc.adjust(-1, -1 ,1 ,1);

  if (render)
  {
    setSetFont(fontId, painter);
    setSetFontColor(fontId, painter);
    painter->drawText(trc, Qt::AlignCenter, text);
  }
  //painter->drawRect(trc);

  return trc;
}

int SmartLabeling::getAlign(int align)
{
  switch (align)
  {
    case SL_AL_CENTER:
      return RT_CENTER;

    case SL_AL_BOTTOM_RIGHT:
      return RT_BOTTOM_RIGHT;

    case SL_AL_TOP_RIGHT:
      return RT_TOP_RIGHT;

    case SL_AL_BOTTOM_LEFT:
      return RT_BOTTOM_LEFT;

    case SL_AL_TOP_LEFT:
      return RT_TOP_LEFT;

    default:
      qDebug() << "SmartLabeling::getAlign() invalid align!!!";
  }

  return RT_CENTER;
}

void SmartLabeling::render(CSkPainter *painter)
{
  int maxSize = 32000;

  if (1)
  {
    foreach (const SmartLabel &label, m_list)
    {
      setSetFont(label.m_fontId, painter);
      setSetFontColor(label.m_fontId, painter);

      renderLabel(painter, label.m_point, label.m_distance, label.m_text, label.m_fontId, label.m_defaultAlign, true);
    }
  }
  else
  {
    qSort(m_list.begin(), m_list.end(), sortFnc);
    QList <QRect> doneList;

    foreach (const SmartLabel &label, m_list)
    {
      if (label.m_allowedAlign != SL_AL_FIXED)
      {
        break;
      }
      doneList << renderLabel(painter, label.m_point, label.m_distance, label.m_text, label.m_fontId, getAlign(label.m_defaultAlign), true);
    }

    foreach (const SmartLabel &label, m_list)
    {
      if (label.m_allowedAlign == SL_AL_FIXED)
      {
        continue;
      }

      bool done = false;

      for (int a = SL_AL_BOTTOM_LEFT - 1; a <= SL_AL_TOP_RIGHT; a++)
      {
        int align;

        if (done)
        {
          break;
        }

        if (a == 1)
          align = getAlign(label.m_defaultAlign);
        else
          align = getAlign(a);

        for (double d = 0; d < label.m_distance; d++)
        {
          QRect rc = renderLabel(painter, label.m_point, label.m_distance + d, label.m_text, label.m_fontId, align, false);

          int hits = 0;
          foreach (const QRect &rect, doneList)
          {
            if (rect.intersects(rc))
            {
              hits++;
            }
          }

          if (hits == 0)
          {
            doneList << renderLabel(painter, label.m_point, label.m_distance + d, label.m_text, label.m_fontId, align, true);
            done = true;
            break;
          }
        }
      }

      if (!done)
      {
        doneList << renderLabel(painter, label.m_point, label.m_distance, label.m_text, label.m_fontId, getAlign(label.m_defaultAlign), true);
      }
    }
  }
}

