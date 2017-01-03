/************************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2016

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#include "crts.h"
#include "planetreport.h"
#include "skutils.h"
#include "castro.h"
#include "constellation.h"
#include "mapobj.h"

PlanetReport::PlanetReport(const mapView_t *view)
{
  m_view = *view;
}

QString PlanetReport::generate()
{
  QString str;
  const QString newLine = "<br/>";

  str += QString(tr("Planet report on %1")).arg(getStrDate(m_view.jd, m_view.geo.tz));
  str += newLine;

  CAstro ast;

  for (int i = 0; i < PT_PLANET_COUNT; i++)
  {
    orbit_t o;

    ast.setParam(&m_view);
    ast.calcPlanet(i, &o);
    rts_t rts;
    CRts  cRTS;

    cRTS.calcOrbitRTS(&rts, i, MO_PLANET, &m_view);

    str += QString(tr("%1 is in constellation %2")).arg(ast.getName(i)).arg(constGetName(constWhatConstel(o.lRD.Ra, o.lRD.Dec, m_view.jd), 1));
    str += newLine;
    str += QString(tr("Magnitude is %1")).arg(getStrMag(o.mag));
    str += newLine;
    str += QString(tr("Apparent diameter is %1")).arg(getStrNumber("", o.sx, 1, "\""));
    str += newLine;
    str += getStrTime(rts.transit, m_view.geo.tz);
  }

  return str;
}
