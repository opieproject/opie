TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= loopcontrol.h mediaplayerplugininterface.h playlistselection.h mediaplayerstate.h \
		  videowidget.h audiowidget.h playlistwidget.h mediaplayer.h audiodevice.h
SOURCES		= main.cpp \
		  loopcontrol.cpp playlistselection.cpp mediaplayerstate.cpp \
		  videowidget.cpp audiowidget.cpp playlistwidget.cpp mediaplayer.cpp audiodevice.cpp
TARGET		= mpegplayer
INCLUDEPATH	+= $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe -lpthread

# INTERFACES	= 
# INCLUDEPATH += $(QPEDIR)/include
# CONFIG+=static
# TMAKE_CXXFLAGS += -DQPIM_STANDALONE
# LIBS		+= libmpeg3/libmpeg3.a -lpthread
# LIBS		+= $(QPEDIR)/plugins/codecs/liblibmadplugin.so

TRANSLATIONS = ../i18n/de/mpegplayer.ts
