DESTDIR		= ../bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= amigo.h \
		  go.h \
		  goplayutils.h \
		  gowidget.h
SOURCES		= amigo.c \
		goplayer.c \
		goplayutils.c \
		killable.c \
		gowidget.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= go

TRANSLATIONS = ../i18n/de/go.ts
TRANSLATIONS = ../i18n/pt_BR/go.ts

