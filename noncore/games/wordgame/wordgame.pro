TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= wordgame.h
SOURCES		= main.cpp \
		  wordgame.cpp
INTERFACES	= newgamebase.ui rulesbase.ui
TARGET		= wordgame
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/wordgame.ts
TRANSLATIONS += ../i18n/pt_BR/wordgame.ts
