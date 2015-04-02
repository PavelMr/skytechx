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

#ifndef CDEMONSTRATION_H
#define CDEMONSTRATION_H

#include <QWidget>
#include <QList>
#include <QEasingCurve>
#include <QTimer>

typedef struct
{
  int    delay;
  double x, y;
  double fov;
  double jd;
  double rot;
} curvePoint_t;

class CDemonstration : public QObject
{
  Q_OBJECT
public:
  explicit CDemonstration(void);
  ~CDemonstration(void);
  void setupPoints();
  void start();
  void stop();
  void pause();
  void rewind();

private:
  QEasingCurve *m_curve;
  QTimer       *m_timer;
  double        m_progress;

  QList <curvePoint_t> m_points;
  bool m_loop;

signals:
  void sigAnimChanged(curvePoint_t &point);

public slots:

  void slotTimer();

};

#endif // CDEMONSTRATION_H
