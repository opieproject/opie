TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(QPEDIR)/bin
HEADERS		= ledmeter.h  sprites.h  toplevel.h  view.h
SOURCES		= ledmeter.cpp  toplevel.cpp  view.cpp main.cpp
TARGET		= qasteroids
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/qasteroids.ts
