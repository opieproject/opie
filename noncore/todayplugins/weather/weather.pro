TEMPLATE = lib
#CONFIG -= moc
CONFIG += qt plugin release

HEADERS = weatherplugin.h       \
          weatherpluginimpl.h   \
          weatherpluginwidget.h \
          weatherconfig.h

SOURCES = weatherplugin.cpp       \
          weatherpluginimpl.cpp   \
          weatherpluginwidget.cpp \
          weatherconfig.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayweatherplugin

TRANSLATIONS = ../../../i18n/de/libtodayweatherplugin.ts \
	 ../../../i18n/nl/libtodayweatherplugin.ts \
         ../../../i18n/xx/libtodayweatherplugin.ts \
         ../../../i18n/en/libtodayweatherplugin.ts \
         ../../../i18n/es/libtodayweatherplugin.ts \
         ../../../i18n/fr/libtodayweatherplugin.ts \
         ../../../i18n/hu/libtodayweatherplugin.ts \
         ../../../i18n/ja/libtodayweatherplugin.ts \
         ../../../i18n/ko/libtodayweatherplugin.ts \
         ../../../i18n/no/libtodayweatherplugin.ts \
         ../../../i18n/pl/libtodayweatherplugin.ts \
         ../../../i18n/pt/libtodayweatherplugin.ts \
         ../../../i18n/pt_BR/libtodayweatherplugin.ts \
         ../../../i18n/sl/libtodayweatherplugin.ts \
         ../../../i18n/zh_CN/libtodayweatherplugin.ts \
         ../../../i18n/zh_TW/libtodayweatherplugin.ts \
         ../../../i18n/it/libtodayweatherplugin.ts \
         ../../../i18n/da/libtodayweatherplugin.ts
include ( $(OPIEDIR)/include.pro )
