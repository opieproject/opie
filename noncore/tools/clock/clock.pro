TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= clock.h
SOURCES		= clock.cpp \
		  main.cpp
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe
INTERFACES	= 
TARGET		= clock

TRANSLATIONS = ../i18n/de/clock.ts
