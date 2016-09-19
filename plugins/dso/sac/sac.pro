#-------------------------------------------------
#
# Project created by QtCreator 2013-08-09T16:33:20
#
#-------------------------------------------------

QT       -= gui

CONFIG   += plugin

INCLUDEPATH += "../../.."


win32:contains(QMAKE_HOST.arch, x86_64) {
  win32-msvc2013:TARGET = "sac_x64"
} else {
  win32-msvc2013:TARGET = "sac"
}

TARGET = $$qtLibraryTarget($$TARGET)

TEMPLATE = lib

SOURCES += sac.cpp

HEADERS += sac.h
