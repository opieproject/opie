TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= ../bin
HEADERS		= minefield.h \
		    gtetrix.h \
		    qtetrix.h \
		    qtetrixb.h \
		    tpiece.h
SOURCES		= main.cpp \
		    gtetrix.cpp \
		    qtetrix.cpp \
		    qtetrixb.cpp \
		    tpiece.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../i18n/pt_BR/tetrix.ts
