#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T09:42:37
#
#-------------------------------------------------

QT       += core gui network webkit webkitwidgets xml sql widgets printsupport script multimedia

CONFIG   += openssl-linked
CONFIG   += qxt
CONFIG   += axcontainer
CONFIG   += plugin
CONFIG   += c++11

DEFINES += NOMINMAX

#Release:DEFINES  += QT_NO_DEBUG_OUTPUT QT_FATAL_WARNINGS

Release:win32-g++ : QMAKE_CXXFLAGS += -fopenmp -O1 -std=c++11  -Wl,--stack,16777216
Debug:win32-g++   : QMAKE_CXXFLAGS += -Wall -Wextra -fopenmp  -Wl,--stack,16777216

win32-g++         : LIBS += -lgomp -lpsapi -lwinmm -lws2_32 -lcrypt32
win32-msvc2013    : LIBS += winmm.lib

#-lssl -lcrypto

Release:win32-msvc2013 :  QMAKE_CXXFLAGS += /openmp /O2 /GL /GS- /GA /FAcs

Release:win32-msvc2013 :  QMAKE_LFLAGS += /LTCG /MAP
Debug:win32-msvc2013   :  QMAKE_LFLAGS += /INCREMENTAL:NO


TARGET = skytech_x
TEMPLATE = app


DEPENDPATH += rc
DEPENDPATH += ssl

#Release: build_nr.commands = $$PWD/build_inc.bat
#Release: build_nr.depends = FORCE
#Release: QMAKE_EXTRA_TARGETS += build_nr
#Release: PRE_TARGETDEPS += build_nr

INCLUDEPATH = core \
              de404 \
              vsop87 \
              elp2000 \
              libsgp4 \
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
    ceditsearch.cpp \
    csaveimage.cpp \
    cnamemagview.cpp \
    ctipofday.cpp \
    caddcustomobject.cpp \
    cucac4.cpp \
    csgp4.cpp \
    csatellitedlg.cpp \
    csatellitesearch.cpp \
    csearchwidget.cpp \
    libsgp4/CoordGeodetic.cpp \
    libsgp4/CoordTopocentric.cpp \
    libsgp4/DateTime.cpp \
    libsgp4/Eci.cpp \
    libsgp4/Globals.cpp \
    libsgp4/Observer.cpp \
    libsgp4/OrbitalElements.cpp \
    libsgp4/SGP4.cpp \
    libsgp4/SolarPosition.cpp \
    libsgp4/TimeSpan.cpp \
    libsgp4/Tle.cpp \
    libsgp4/Util.cpp \
    libsgp4/Vector.cpp \
    clog.cpp \
    csearchdsocat.cpp \
    cversioncheck.cpp \
    cgamepad.cpp \
    chorizoneditor.cpp \
    chorizonwidget.cpp \
    csatelliteevent.cpp \
    creleseinfo.cpp \
    suntexture.cpp \
    cdonation.cpp \
    cdssmanager.cpp \
    c3dsolar.cpp \
    c3dsolarwidget.cpp \
    cbinocular.cpp \
    cbinocularedit.cpp \
    skmapcontrol.cpp \
    cinsertfinder.cpp \
    cfinderedit.cpp \
    dssheaderdialog.cpp \
    cdownloadfile.cpp \
    moonlessnightsdlg.cpp \
    systemsettings.cpp \
    xmlattrparser.cpp \
    nutation.cpp \
    smartlabeling.cpp \
    clunarfeaturessearch.cpp \
    cplanetsize.cpp \
    usnob1.cpp \
    urat1.cpp \
    cstarexample.cpp \
    cadvsearch.cpp \
    nomad.cpp \
    skwatch.cpp \
    soundmanager.cpp \
    skserver.cpp \
    qcustomplot.cpp \
    astcomdowntypedlg.cpp \
    skprogressdialog.cpp \
    vsop87/venus_vsop87.cpp \
    vsop87/vsop87.cpp \
    vsop87/earth_vsop87.cpp \
    vsop87/mercury_vsop87.cpp \
    vsop87/jupiter_vsop87.cpp \
    vsop87/mars_vsop87.cpp \
    vsop87/neptune_vsop87.cpp \
    vsop87/saturn_vsop87.cpp \
    vsop87/uranus_vsop87.cpp \
    cskeventdocdialog.cpp \
    skeventdocument.cpp \
    elp2000/elp2000.cpp

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
    build.h \
    csaveimage.h \
    cnamemagview.h \
    ctipofday.h \
    caddcustomobject.h \
    cucac4.h \
    csgp4.h \
    csatellitedlg.h \
    csatellitesearch.h \
    csearchwidget.h \
    build_inc.h \
    libsgp4/CoordGeodetic.h \
    libsgp4/CoordTopocentric.h \
    libsgp4/DateTime.h \
    libsgp4/DecayedException.h \
    libsgp4/Eci.h \
    libsgp4/Globals.h \
    libsgp4/Observer.h \
    libsgp4/OrbitalElements.h \
    libsgp4/SatelliteException.h \
    libsgp4/SGP4.h \
    libsgp4/SolarPosition.h \
    libsgp4/TimeSpan.h \
    libsgp4/Tle.h \
    libsgp4/TleException.h \
    libsgp4/Util.h \
    libsgp4/Vector.h \
    clog.h \
    csearchdsocat.h \
    cversioncheck.h \
    cgamepad.h \
    chorizoneditor.h \
    chorizonwidget.h \
    csatelliteevent.h \
    creleseinfo.h \
    suntexture.h \
    cdonation.h \
    cdssmanager.h \
    c3dsolar.h \
    c3dsolarwidget.h \
    cbinocular.h \
    cbinocularedit.h \
    skmapcontrol.h \
    cinsertfinder.h \
    cfinderedit.h \
    dssheaderdialog.h \
    cdownloadfile.h \
    moonlessnightsdlg.h \
    systemsettings.h \
    xmlattrparser.h \
    nutation.h \
    smartlabeling.h \
    clunarfeaturessearch.h \
    cplanetsize.h \
    usnob1.h \
    urat1.h \
    cstarexample.h \
    cadvsearch.h \
    tass17.h \
    nomad.h \
    skcalendar.h \
    skwatch.h \
    soundmanager.h \
    skserver.h \
    qcustomplot.h \
    astcomdowntypedlg.h \
    skprogressdialog.h \
    vsop87/vsop87.h \
    cskeventdocdialog.h \
    skeventdocument.h \
    elp2000/elp2000.h


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
    cweather.ui \
    csaveimage.ui \
    ctipofday.ui \
    caddcustomobject.ui \
    csatellitedlg.ui \
    csatellitesearch.ui \
    csearchwidget.ui \
    csearchdsocat.ui \
    cversioncheck.ui \
    chorizoneditor.ui \
    csatelliteevent.ui \
    creleseinfo.ui \
    cdonation.ui \
    cdssmanager.ui \
    c3dsolar.ui \
    cbinocular.ui \
    cbinocularedit.ui \
    cinsertfinder.ui \
    cfinderedit.ui \
    dssheaderdialog.ui \
    cdownloadfile.ui \
    moonlessnightsdlg.ui \
    clunarfeaturessearch.ui \
    cplanetsize.ui \
    cadvsearch.ui \
    astcomdowntypedlg.ui \
    cskeventdocdialog.ui


RESOURCES += \
    resource.qrc

RC_FILE = skytech_rc.rc

TRANSLATIONS = english.ts \
               cesky.ts

OTHER_FILES += \
    skytech_rc.rc

DISTFILES += \
    libsgp4/Makefile.am \
    libsgp4/Makefile.in



