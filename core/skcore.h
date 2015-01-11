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

