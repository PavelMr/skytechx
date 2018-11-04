/***********************************************************************
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
#ifndef PLANETREPORT_H
#define PLANETREPORT_H

#include "cmapview.h"

#include <QObject>

class PlanetReport : public QObject
{
  Q_OBJECT
public:
  PlanetReport(const mapView_t *view);
  QString generate();

private:
  mapView_t m_view;
};

#endif // PLANETREPORT_H
