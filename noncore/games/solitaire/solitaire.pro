TEMPLATE	= app

CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= canvascard.h   canvasshapes.h   cardgame.h   cardgamelayout.h   cardpile.h   card.h   carddeck.h   canvascardgame.h   freecellcardgame.h   patiencecardgame.h   canvascardwindow.h

SOURCES		= canvascard.cpp canvasshapes.cpp cardgame.cpp cardgamelayout.cpp cardpile.cpp card.cpp carddeck.cpp canvascardgame.cpp freecellcardgame.cpp patiencecardgame.cpp canvascardwindow.cpp main.cpp

TARGET		= patience

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

REQUIRES	= patience

TRANSLATIONS = ../i18n/de/patience.ts
