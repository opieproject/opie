DESTDIR		= $(OPIEDIR)/bin
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

TRANSLATIONS = ../../../i18n/de/go.ts \
	 ../../../i18n/da/go.ts \
	 ../../../i18n/xx/go.ts \
	 ../../../i18n/en/go.ts \
	 ../../../i18n/es/go.ts \
	 ../../../i18n/fr/go.ts \
	 ../../../i18n/hu/go.ts \
	 ../../../i18n/ja/go.ts \
	 ../../../i18n/ko/go.ts \
	 ../../../i18n/no/go.ts \
	 ../../../i18n/pl/go.ts \
	 ../../../i18n/pt/go.ts \
	 ../../../i18n/pt_BR/go.ts \
	 ../../../i18n/sl/go.ts \
	 ../../../i18n/zh_CN/go.ts \
	 ../../../i18n/zh_TW/go.ts
