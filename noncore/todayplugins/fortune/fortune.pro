TEMPLATE = lib
#CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = fortuneplugin.h fortunepluginimpl.h \
	fortunepluginwidget.h
SOURCES = fortuneplugin.cpp fortunepluginimpl.cpp \
	fortunepluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayfortuneplugin

TRANSLATIONS = ../../../i18n/de/libtodayfortuneplugin.ts \
	 ../../../i18n/nl/libtodayfortuneplugin.ts \
         ../../../i18n/xx/libtodayfortuneplugin.ts \
         ../../../i18n/en/libtodayfortuneplugin.ts \
         ../../../i18n/es/libtodayfortuneplugin.ts \
         ../../../i18n/fr/libtodayfortuneplugin.ts \
         ../../../i18n/hu/libtodayfortuneplugin.ts \
         ../../../i18n/ja/libtodayfortuneplugin.ts \
         ../../../i18n/ko/libtodayfortuneplugin.ts \
         ../../../i18n/no/libtodayfortuneplugin.ts \
         ../../../i18n/pl/libtodayfortuneplugin.ts \
         ../../../i18n/pt/libtodayfortuneplugin.ts \
         ../../../i18n/pt_BR/libtodayfortuneplugin.ts \
         ../../../i18n/sl/libtodayfortuneplugin.ts \
         ../../../i18n/zh_CN/libtodayfortuneplugin.ts \
         ../../../i18n/zh_TW/libtodayfortuneplugin.ts \
         ../../../i18n/it/libtodayfortuneplugin.ts \
         ../../../i18n/da/libtodayfortuneplugin.ts


include ( $(OPIEDIR)/include.pro )
