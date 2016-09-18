/***********************************************************************
This file is part of SkytechX.

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

#ifndef SK_CORE_H
#define SK_CORE_H

#include <QtCore>
#include <QtWidgets>

#define ALIGNED(x)   __attribute__ ((aligned (x)))

#include "const.h"
#include "skmath.h"
#include "skmatrix.h"
#include "vecmath.h"
#include "plane.h"
#include "bbox.h"
#include "skutils.h"
#include "skdebug.h"
#include "skfile.h"
#include "../cconsole.h"

QString readAllFile(QString name);
void checkAndCreateFolder(const QString &folder);
void copyFile(const QString &source, const QString &dest);

#endif

