TEMPLATE  = app
CONFIG    = qt warn_on release
#release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = loopcontrol.h mediaplayerplugininterface.h \
	playlistselection.h mediaplayerstate.h \
	videowidget.h audiowidget.h playlistwidget.h \
	mediaplayer.h audiodevice.h inputDialog.h om3u.h
SOURCES   = main.cpp \
      loopcontrol.cpp playlistselection.cpp mediaplayerstate.cpp \
      videowidget.cpp audiowidget.cpp playlistwidget.cpp \
      mediaplayer.cpp audiodevice.cpp inputDialog.cpp om3u.cpp
TARGET    = opieplayer
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lpthread -lopie

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
