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
TRANSLATIONS += ../i18n/pt_BR/go.ts
TRANSLATIONS   += ../i18n/en/go.ts
TRANSLATIONS   += ../i18n/hu/go.ts
TRANSLATIONS   += ../i18n/ja/go.ts
TRANSLATIONS   += ../i18n/fr/go.ts
TRANSLATIONS   += ../i18n/sl/go.ts
TRANSLATIONS   += ../i18n/pl/go.ts
TRANSLATIONS   += ../i18n/ko/go.ts
TRANSLATIONS   += ../i18n/no/go.ts
TRANSLATIONS   += ../i18n/zh_CN/go.ts
TRANSLATIONS   += ../i18n/zh_TW/go.ts
