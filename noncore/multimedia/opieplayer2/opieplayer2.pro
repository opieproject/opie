TEMPLATE  = app
CONFIG    = qt warn_on release
#release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = playlistselection.h mediaplayerstate.h \
	      videowidget.h audiowidget.h playlistwidget.h mediaplayer.h inputDialog.h
SOURCES   = main.cpp \
    	  playlistselection.cpp mediaplayerstate.cpp \
     	 videowidget.cpp audiowidget.cpp playlistwidget.cpp mediaplayer.cpp inputDialog.cpp
TARGET    = opieplayer
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lpthread -lopie

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

