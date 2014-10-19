#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T09:42:37
#
#-------------------------------------------------

QT       += core gui network webkit webkitwidgets xml sql widgets printsupport

CONFIG   += axcontainer
CONFIG   += plugin
CONFIG   += c++11

DEFINES  += QT_NO_DEBUG_OUTPUT

Release:win32-g++ : QMAKE_CXXFLAGS += -march=native -O3 -ffast-math -fopenmp -fomit-frame-pointer -momit-leaf-frame-pointer -flto -fwhole-program -ftree-loop-distribution
Debug:win32-g++ : QMAKE_CXXFLAGS += -Wall -Wextra -fopenmp
#win32-g++ : LIBS += -lgomp -lpsapi
win32-g++ : LIBS += -lgomp

#win32-msvc2010 :  QMAKE_CXXFLAGS += /openmp /arch:SSE /arch:SSE2 /O2 /GL /GS- /GA
#win32-msvc2010 :  QMAKE_LFLAGS += /LTCG

TARGET = skytech_x
TEMPLATE = app

INCLUDEPATH = core \
              de404 \
              $$PWD \


SOURCES += main.cpp\
        mainwindow.cpp \
    core/vecmath.cpp \
    core/skmatrix.cpp \
    cmapview.cpp \
    core/plane.cpp \
    tycho.cpp \
    cgscreg.cpp \
    cstarrenderer.cpp \
    transform.cpp \
    Gsc.cpp \
    jd.cpp \
    cskpainter.cpp \
    skymap.cpp \
    skmath.cpp \
    setting.cpp \
    cabout.cpp \
    core/skcore.cpp \
    constellation.cpp \
    precess.cpp \
    cgrid.cpp \
    cdso.cpp \
    cloadingdlg.cpp \
    core/skutils.cpp \
    cshape.cpp \
    cscanrender.cpp \
    cstatusbar.cpp \
    cppmxl.cpp \
    mapobj.cpp \
    cfits.cpp \
    cbkimages.cpp \
    castro.cpp \
    csetjd.cpp \
    csettime.cpp \
    cdeltat.cpp \
    cobjfillinfo.cpp \
    clvqinfo.cpp \
    smap.cpp \
    cobjinfo.cpp \
    cdlgsrchstarname.cpp \
    cdsocomnamesearch.cpp \
    de404/VEN404.cpp \
    de404/URA404.cpp \
    de404/SAT404.cpp \
    de404/NEP404.cpp \
    de404/MER404.cpp \
    de404/MAR404.cpp \
    de404/JUP404.cpp \
    de404/EAR404.cpp \
    de404/de404.cpp \
    cplanetrenderer.cpp \
    csearch.cpp \
    csetposition.cpp \
    cmilkyway.cpp \
    ctristatebutton.cpp \
    cconsole.cpp \
    cdaynight.cpp \
    csohoimg.cpp \
    cimageview.cpp \
    curlfile.cpp \
    cplanetvis.cpp \
    cmooncal.cpp \
    crts.cpp \
    cchartedit.cpp \
    cdssdlg.cpp \
    cmultiprogress.cpp \
    cdownload.cpp \
    cwpossel.cpp \
    ctextsel.cpp \
    cteleplug.cpp \
    cselteleplug.cpp \
    cdrawing.cpp \
    cseltelefield.cpp \
    cimagemanip.cpp \
    csatxyz.cpp \
    casterdlg.cpp \
    castcomsearch.cpp \
    cdownloadmpc.cpp \
    csavetm.cpp \
    crestoretm.cpp \
    casteredit.cpp \
    clineeditcomp.cpp \
    cdailyev.cpp \
    cwposmap.cpp \
    earthtools/cparse.cpp \
    earthtools/cearthtools.cpp \
    mlibration.cpp \
    csatevents.cpp \
    cgalery.cpp \
    cdb.cpp \
    ctychosearch.cpp \
    ceventsearch.cpp \
    ceventprogdlg.cpp \
    de404/moon.cpp \
    ccomdlg.cpp \
    ccomedit.cpp \
    cobjtracking.cpp \
    background.cpp \
    ctracklist.cpp \
    clunarfeatures.cpp \
    clfmodel.cpp \
    cfontcolordlg.cpp \
    clinecolordlg.cpp \
    cteleeditdlg.cpp \
    cdbstarsdlg.cpp \
    cwaitlogo.cpp \
    csimplelist.cpp \
    cplnsearch.cpp \
    cinserttext.cpp \
    cinsertfrmfield.cpp \
    czoombar.cpp \
    cstylesel.cpp \
    csetting.cpp \
    cfontcolorbutton.cpp \
    cpushcolorbutton.cpp \
    cframegradient.cpp \
    cfrmedit.cpp \
    Usno2A.cpp \
    cinsertcircle.cpp \
    cephlist.cpp \
    cephtable.cpp \
    dsoplug.cpp \
    cgeohash.cpp \
    cpolarishourangle.cpp \
    CRADecSpinBox.cpp \
    ctimewidget.cpp \
    core/skdebug.cpp \
    core/skfile.cpp \
    core/skconfigproperty.cpp \
    cdsocatalogue.cpp \
    cpixmapwidget.cpp \
    cgetprofile.cpp \
    cdsogalerylist.cpp \
    csethorizon.cpp \
    csellanguage.cpp \
    clistwidget.cpp \
    ctimelapsepref.cpp \
    cxmlsimplemapparser.cpp \
    csolarsystem.cpp \
    csearchconst.cpp \
    ctimedialog.cpp \
    cearthmapview.cpp \
    cdrawinglist.cpp \
    cdemonstration.cpp \
    csignspinbox.cpp \
    cplanetaltitude.cpp \
    cweather.cpp \
    ceditsearch.cpp

HEADERS  += mainwindow.h \
    core/vecmath.h \
    core/skmatrix.h \
    core/skcore.h \
    cmapview.h \
    core/plane.h \
    core/bbox.h \
    core/const.h \
    tycho.h \
    cgscreg.h \
    cstarrenderer.h \
    transform.h \
    Gsc.h \
    jd.h \
    cskpainter.h \
    skymap.h \
    core/skmath.h \
    setting.h \
    cabout.h \
    constellation.h \
    precess.h \
    cgrid.h \
    cdso.h \
    dso_def.h \
    cloadingdlg.h \
    core/skutils.h \
    cshape.h \
    cscanrender.h \
    cstatusbar.h \
    cppmxl.h \
    mapobj.h \
    cfits.h \
    cbkimages.h \
    castro.h \
    csetjd.h \
    csettime.h \
    cdeltat.h \
    cobjfillinfo.h \
    clvqinfo.h \
    smap.h \
    cobjinfo.h \
    cdlgsrchstarname.h \
    cdsocomnamesearch.h \
    de404/plantbl.h \
    cplanetrenderer.h \
    csearch.h \
    csetposition.h \
    cmilkyway.h \
    ctristatebutton.h \
    cconsole.h \
    cdaynight.h \
    csohoimg.h \
    cimageview.h \
    curlfile.h \
    cplanetvis.h \
    cmooncal.h \
    crts.h \
    cchartedit.h \
    cdssdlg.h \
    cmultiprogress.h \
    cdownload.h \
    cwpossel.h \
    ctextsel.h \
    cteleplug.h \
    cselteleplug.h \
    cdrawing.h \
    cseltelefield.h \
    cimagemanip.h \
    csatxyz.h \
    casterdlg.h \
    castcomsearch.h \
    cdownloadmpc.h \
    csavetm.h \
    crestoretm.h \
    casteredit.h \
    clineeditcomp.h \
    cdailyev.h \
    cwposmap.h \
    earthtools/cparse.h \
    earthtools/cearthtools.h \
    csatevents.h \
    cgalery.h \
    cdb.h \
    ctychosearch.h \
    ceventsearch.h \
    ceventprogdlg.h \
    ccomdlg.h \
    ccomedit.h \
    cobjtracking.h \
    background.h \
    ctracklist.h \
    clunarfeatures.h \
    clfmodel.h \
    cfontcolordlg.h \
    clinecolordlg.h \
    cteleeditdlg.h \
    cdbstarsdlg.h \
    cwaitlogo.h \
    csimplelist.h \
    cplnsearch.h \
    cinserttext.h \
    cinsertfrmfield.h \
    czoombar.h \
    cstylesel.h \
    csetting.h \
    cfontcolorbutton.h \
    cpushcolorbutton.h \
    cframegradient.h \
    cfrmedit.h \
    Usno2A.h \
    cinsertcircle.h \
    cephlist.h \
    cephtable.h \
    dsoplug.h \
    cgeohash.h \
    cpolarishourangle.h \
    CRADecSpinBox.h \
    ctimewidget.h \
    core/skdebug.h \
    core/skfile.h \
    cdsoplugininterface.h \
    core/skconfigproperty.h \
    cdsocatalogue.h \
    cpixmapwidget.h \
    cgetprofile.h \
    cdsogalerylist.h \
    csethorizon.h \
    csellanguage.h \
    clistwidget.h \
    ctimelapsepref.h \
    cxmlsimplemapparser.h \
    csolarsystem.h \
    csearchconst.h \
    ctimedialog.h \
    cearthmapview.h \
    cdrawinglist.h \
    cdemonstration.h \
    csignspinbox.h \
    cplanetaltitude.h \
    cweather.h \
    ceditsearch.h \
    build.h

FORMS    += mainwindow.ui \
    cabout.ui \
    cloadingdlg.ui \
    csetjd.ui \
    csettime.ui \
    cdeltat.ui \
    cobjinfo.ui \
    cdlgsrchstarname.ui \
    cdsocomnamesearch.ui \
    csetposition.ui \
    cdaynight.ui \
    csohoimg.ui \
    cplanetvis.ui \
    cmooncal.ui \
    cdssdlg.ui \
    cwpossel.ui \
    ctextsel.ui \
    cselteleplug.ui \
    cseltelefield.ui \
    casterdlg.ui \
    castcomsearch.ui \
    cdownloadmpc.ui \
    csavetm.ui \
    crestoretm.ui \
    casteredit.ui \
    cdailyev.ui \
    cwposmap.ui \
    csatevents.ui \
    cgalery.ui \
    ctychosearch.ui \
    ceventsearch.ui \
    ceventprogdlg.ui \
    ccomdlg.ui \
    ccomedit.ui \
    cobjtracking.ui \
    ctracklist.ui \
    cfontcolordlg.ui \
    clinecolordlg.ui \
    cteleeditdlg.ui \
    cdbstarsdlg.ui \
    cplnsearch.ui \
    cinserttext.ui \
    cinsertfrmfield.ui \
    cstylesel.ui \
    csetting.ui \
    cfrmedit.ui \
    cinsertcircle.ui \
    cephlist.ui \
    cephtable.ui \
    cpolarishourangle.ui \
    ctimewidget.ui \
    cdsocatalogue.ui \
    cgetprofile.ui \
    cdsogalerylist.ui \
    csethorizon.ui \
    csellanguage.ui \
    ctimelapsepref.ui \
    csolarsystem.ui \
    csearchconst.ui \
    ctimedialog.ui \
    cdrawinglist.ui \
    cplanetaltitude.ui \
    cweather.ui


RESOURCES += \
    resource.qrc

RC_FILE = skytech_rc.rc

TRANSLATIONS = english.ts \
               cesky.ts



