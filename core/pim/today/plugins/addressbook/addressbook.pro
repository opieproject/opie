TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = addressplugin.h addresspluginimpl.h addresspluginconfig.h  \
	addresspluginwidget.h
SOURCES = addressplugin.cpp addresspluginimpl.cpp addresspluginconfig.cpp \
	addresspluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayaddressbookplugin

TRANSLATIONS = ../../../../../i18n/de/libtodayaddressbookplugin.ts \
         ../../../../../i18n/xx/libtodayaddressbookplugin.ts \
         ../../../../../i18n/en/libtodayaddressbookplugin.ts \
         ../../../../../i18n/es/libtodayaddressbookplugin.ts \
         ../../../../../i18n/fr/libtodayaddressbookplugin.ts \
         ../../../../../i18n/hu/libtodayaddressbookplugin.ts \
         ../../../../../i18n/ja/libtodayaddressbookplugin.ts \
         ../../../../../i18n/ko/libtodayaddressbookplugin.ts \
         ../../../../../i18n/no/libtodayaddressbookplugin.ts \
         ../../../../../i18n/pl/libtodayaddressbookplugin.ts \
         ../../../../../i18n/pt/libtodayaddressbookplugin.ts \
         ../../../../../i18n/pt_BR/libtodayaddressbookplugin.ts \
         ../../../../../i18n/sl/libtodayaddressbookplugin.ts \
         ../../../../../i18n/zh_CN/libtodayaddressbookplugin.ts \
         ../../../../../i18n/zh_TW/libtodayaddressbookplugin.ts \
         ../../../../../i18n/it/libtodayaddressbookplugin.ts \
         ../../../../../i18n/da/libtodayaddressbookplugin.ts




include ( $(OPIEDIR)/include.pro )
