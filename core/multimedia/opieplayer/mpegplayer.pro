TEMPLATE  = app
CONFIG    = qt warn_on release
#release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = loopcontrol.h mediaplayerplugininterface.h playlistselection.h mediaplayerstate.h \
      videowidget.h audiowidget.h playlistwidget.h mediaplayer.h audiodevice.h inputDialog.h
SOURCES   = main.cpp \
      loopcontrol.cpp playlistselection.cpp mediaplayerstate.cpp \
      videowidget.cpp audiowidget.cpp playlistwidget.cpp mediaplayer.cpp audiodevice.cpp inputDialog.cpp
TARGET    = mpegplayer
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lpthread

# INTERFACES  = 
# INCLUDEPATH += $(OPIEDIR)/include
# CONFIG+=static
# TMAKE_CXXFLAGS += -DQPIM_STANDALONE
# LIBS    += libmpeg3/libmpeg3.a -lpthread
# LIBS    += $(OPIEDIR)/plugins/codecs/liblibmadplugin.so

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

TRANSLATIONS = ../i18n/de/mpegplayer.ts
TRANSLATIONS += ../i18n/pt_BR/mpegplayer.ts
TRANSLATIONS   += ../i18n/en/mpegplayer.ts
TRANSLATIONS   += ../i18n/hu/mpegplayer.ts
TRANSLATIONS   += ../i18n/ja/mpegplayer.ts
TRANSLATIONS   += ../i18n/sl/mpegplayer.ts
TRANSLATIONS   += ../i18n/fr/mpegplayer.ts
TRANSLATIONS   += ../i18n/ko/mpegplayer.ts
TRANSLATIONS   += ../i18n/no/mpegplayer.ts
TRANSLATIONS   += ../i18n/zh_CN/mpegplayer.ts
TRANSLATIONS   += ../i18n/zh_TW/mpegplayer.ts
