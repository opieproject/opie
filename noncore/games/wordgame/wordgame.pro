TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= wordgame.h
SOURCES		= main.cpp \
		  wordgame.cpp
INTERFACES	= newgamebase.ui rulesbase.ui
TARGET		= wordgame
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/wordgame.ts
