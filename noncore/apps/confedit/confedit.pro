DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
#CONFIG		= qt warn_on release
CONFIG		= qt warn_on debug
HEADERS		= mainwindow.h listviewconfdir.h listviewitemconffile.h listviewitemconfigentry.h editwidget.h listviewitemconf.h
SOURCES		= main.cpp mainwindow.cpp listviewconfdir.cpp listviewitemconffile.cpp listviewitemconfigentry.cpp editwidget.cpp listviewitemconf.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/ioclude
LIBS            += -lqpe
INTERFACES	= 
TARGET		= confedit

TRANSLATIONS = ../../../i18n/de/confedit.ts \
	 ../../../i18n/en/confedit.ts \
	 ../../../i18n/es/confedit.ts \
	 ../../../i18n/fr/confedit.ts \
	 ../../../i18n/hu/confedit.ts \
	 ../../../i18n/ja/confedit.ts \
	 ../../../i18n/ko/confedit.ts \
	 ../../../i18n/no/confedit.ts \
	 ../../../i18n/pl/confedit.ts \
	 ../../../i18n/pt/confedit.ts \
	 ../../../i18n/pt_BR/confedit.ts \
	 ../../../i18n/sl/confedit.ts \
	 ../../../i18n/zh_CN/confedit.ts \
	 ../../../i18n/zh_TW/confedit.ts
