TEMPLATE	= app
CONFIG		+= qt warn_on release

DESTDIR		= $(QPEDIR)/bin

HEADERS	= textedit.h

SOURCES	= main.cpp textedit.cpp

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TARGET		= textedit

TRANSLATIONS = ../i18n/de/textedit.ts
