TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = mailplugin.h mailpluginimpl.h mailpluginwidget.h
SOURCES = mailplugin.cpp mailpluginimpl.cpp mailpluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -opiepim2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaymailplugin

TRANSLATIONS = ../../../../../i18n/de/libtodaymailplugin.ts \
	 ../../../../../i18n/nl/libtodaymailplugin.ts \
         ../../../../../i18n/xx/libtodaymailplugin.ts \
         ../../../../../i18n/en/libtodaymailplugin.ts \
         ../../../../../i18n/es/libtodaymailplugin.ts \
         ../../../../../i18n/fr/libtodaymailplugin.ts \
         ../../../../../i18n/hu/libtodaymailplugin.ts \
         ../../../../../i18n/ja/libtodaymailplugin.ts \
         ../../../../../i18n/ko/libtodaymailplugin.ts \
         ../../../../../i18n/no/libtodaymailplugin.ts \
         ../../../../../i18n/pl/libtodaymailplugin.ts \
         ../../../../../i18n/pt/libtodaymailplugin.ts \
         ../../../../../i18n/pt_BR/libtodaymailplugin.ts \
         ../../../../../i18n/sl/libtodaymailplugin.ts \
         ../../../../../i18n/zh_CN/libtodaymailplugin.ts \
         ../../../../../i18n/zh_TW/libtodaymailplugin.ts \
         ../../../../../i18n/it/libtodaymailplugin.ts \
         ../../../../../i18n/da/libtodaymailplugin.ts
include ( $(OPIEDIR)/include.pro )
