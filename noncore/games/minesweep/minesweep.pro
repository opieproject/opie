TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= minefield.h \
		  minesweep.h
SOURCES		= main.cpp \
		  minefield.cpp \
		  minesweep.cpp
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../i18n/de/minesweep.ts
