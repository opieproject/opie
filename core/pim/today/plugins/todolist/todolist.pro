TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin release

# Input
HEADERS = todoplugin.h todopluginimpl.h todopluginconfig.h  \
	todopluginwidget.h
SOURCES = todoplugin.cpp todopluginimpl.cpp todopluginconfig.cpp \
	todopluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaytodolistplugin

TRANSLATIONS = ../../../../../i18n/de/libtodaytodolistplugin.ts \
	 ../../../../../i18n/nl/libtodaytodolistplugin.ts \
         ../../../../../i18n/xx/libtodaytodolistplugin.ts \
         ../../../../../i18n/en/libtodaytodolistplugin.ts \
         ../../../../../i18n/es/libtodaytodolistplugin.ts \
         ../../../../../i18n/fr/libtodaytodolistplugin.ts \
         ../../../../../i18n/hu/libtodaytodolistplugin.ts \
         ../../../../../i18n/ja/libtodaytodolistplugin.ts \
         ../../../../../i18n/ko/libtodaytodolistplugin.ts \
         ../../../../../i18n/no/libtodaytodolistplugin.ts \
         ../../../../../i18n/pl/libtodaytodolistplugin.ts \
         ../../../../../i18n/pt/libtodaytodolistplugin.ts \
         ../../../../../i18n/pt_BR/libtodaytodolistplugin.ts \
         ../../../../../i18n/sl/libtodaytodolistplugin.ts \
         ../../../../../i18n/zh_CN/libtodaytodolistplugin.ts \
         ../../../../../i18n/zh_TW/libtodaytodolistplugin.ts \
         ../../../../../i18n/it/libtodaytodolistplugin.ts \
         ../../../../../i18n/da/libtodaytodolistplugin.ts

include ( $(OPIEDIR)/include.pro )
