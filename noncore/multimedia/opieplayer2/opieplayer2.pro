TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = playlistselection.h mediaplayerstate.h xinecontrol.h \
   	    videowidget.h audiowidget.h playlistwidget.h om3u.h mediaplayer.h inputDialog.h \
            frame.h lib.h xinevideowidget.h volumecontrol.h playlistwidgetgui.h\
            alphablend.h yuv2rgb.h threadutil.h mediawidget.h playlistview.h playlistfileview.h \
	    skin.h
SOURCES   = main.cpp \
            playlistselection.cpp mediaplayerstate.cpp xinecontrol.cpp \
            videowidget.cpp audiowidget.cpp playlistwidget.cpp om3u.cpp mediaplayer.cpp inputDialog.cpp \
            frame.cpp lib.cpp nullvideo.c xinevideowidget.cpp volumecontrol.cpp \
	    playlistwidgetgui.cpp\
            alphablend.c yuv2rgb.c yuv2rgb_arm.c yuv2rgb_arm4l.S \
	    threadutil.cpp mediawidget.cpp playlistview.cpp playlistfileview.cpp \
	    skin.cpp
TARGET    = opieplayer2
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include 
LIBS        += -lqpe -lpthread -lopie -lxine -lstdc++
MOC_DIR   = qpeobj
OBJECTS_DIR = qpeobj

#INCLUDEPATH += $(OPIEDIR)/include
#DEPENDPATH  += $(OPIEDIR)/include



TRANSLATIONS = ../../../i18n/de/opieplayer2.ts \
	 ../../../i18n/nl/opieplayer2.ts \
        ../../../i18n/da/opieplayer2.ts \
        ../../../i18n/xx/opieplayer2.ts \
        ../../../i18n/en/opieplayer2.ts \
        ../../../i18n/es/opieplayer2.ts \
        ../../../i18n/fr/opieplayer2.ts \
        ../../../i18n/hu/opieplayer2.ts \
        ../../../i18n/ja/opieplayer2.ts \
        ../../../i18n/ko/opieplayer2.ts \
        ../../../i18n/no/opieplayer2.ts \
        ../../../i18n/pl/opieplayer2.ts \
        ../../../i18n/pt/opieplayer2.ts \
        ../../../i18n/pt_BR/opieplayer2.ts \
        ../../../i18n/sl/opieplayer2.ts \
        ../../../i18n/zh_CN/opieplayer2.ts \
        ../../../i18n/zh_TW/opieplayer2.ts


include ( $(OPIEDIR)/include.pro )
