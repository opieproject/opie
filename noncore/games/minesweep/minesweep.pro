TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= minefield.h \
		  minesweep.h
SOURCES		= main.cpp \
		  minefield.cpp \
		  minesweep.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../i18n/de/minesweep.ts
