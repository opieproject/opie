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

TRANSLATIONS = ../../../i18n/de/opieplayer.ts \
        ../../../i18n/en/opieplayer.ts \
        ../../../i18n/es/opieplayer.ts \
        ../../../i18n/fr/opieplayer.ts \
        ../../../i18n/hu/opieplayer.ts \
        ../../../i18n/ja/opieplayer.ts \
        ../../../i18n/ko/opieplayer.ts \
        ../../../i18n/no/opieplayer.ts \
        ../../../i18n/pl/opieplayer.ts \
        ../../../i18n/pt/opieplayer.ts \
        ../../../i18n/pt_BR/opieplayer.ts \
        ../../../i18n/sl/opieplayer.ts \
        ../../../i18n/zh_CN/opieplayer.ts \
        ../../../i18n/zh_TW/opieplayer.ts
