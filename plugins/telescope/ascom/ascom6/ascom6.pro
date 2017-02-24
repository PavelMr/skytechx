#-------------------------------------------------
#
# Project created by QtCreator 2013-04-11T17:40:21
#
#-------------------------------------------------


win32:contains(QMAKE_HOST.arch, x86_64) {
  win32-msvc2013:TARGET = "ascom6_x64"
} else {
  win32-msvc2013:TARGET = "ascom6"
  QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,"5.01"
}

TARGET = $$qtLibraryTarget($$TARGET)

QT += axcontainer
TEMPLATE = lib
CONFIG   += axcontainer plugin

SOURCES += ascom6.cpp

HEADERS += ascom6.h \
    ../../../../cteleplugininterface.h


