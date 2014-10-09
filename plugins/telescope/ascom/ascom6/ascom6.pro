#-------------------------------------------------
#
# Project created by QtCreator 2013-04-11T17:40:21
#
#-------------------------------------------------


Debug:TARGET = $$qtLibraryTarget(ascom6) + "_d"
Release:TARGET = $$qtLibraryTarget(ascom6)

QT += axcontainer
TEMPLATE = lib
CONFIG   += axcontainer plugin

SOURCES += ascom6.cpp

HEADERS += ascom6.h \
    ../../../../cteleplugininterface.h


