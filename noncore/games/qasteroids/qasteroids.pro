TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= ledmeter.h  sprites.h  toplevel.h  view.h
SOURCES		= ledmeter.cpp  toplevel.cpp  view.cpp main.cpp
TARGET		= qasteroids
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/qasteroids.ts
TRANSLATIONS = ../i18n/pt_BR/qasteroids.ts
