TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin release
# Input
HEADERS = datebookplugin.h datebookpluginimpl.h datebookpluginconfig.h \
	datebookevent.h datebookpluginwidget.h
SOURCES = datebookplugin.cpp datebookpluginimpl.cpp datebookpluginconfig.cpp \
	datebookevent.cpp datebookpluginwidget.cpp
INTERFACES = datebookpluginconfigbase.ui

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaydatebookplugin

TRANSLATIONS = ../../../../../i18n/de/libtodaydatebookplugin.ts \
	 ../../../../../i18n/nl/libtodaydatebookplugin.ts \
         ../../../../../i18n/xx/libtodaydatebookplugin.ts \
         ../../../../../i18n/en/libtodaydatebookplugin.ts \
         ../../../../../i18n/es/libtodaydatebookplugin.ts \
         ../../../../../i18n/fr/libtodaydatebookplugin.ts \
         ../../../../../i18n/hu/libtodaydatebookplugin.ts \
         ../../../../../i18n/ja/libtodaydatebookplugin.ts \
         ../../../../../i18n/ko/libtodaydatebookplugin.ts \
         ../../../../../i18n/no/libtodaydatebookplugin.ts \
         ../../../../../i18n/pl/libtodaydatebookplugin.ts \
         ../../../../../i18n/pt/libtodaydatebookplugin.ts \
         ../../../../../i18n/pt_BR/libtodaydatebookplugin.ts \
         ../../../../../i18n/sl/libtodaydatebookplugin.ts \
         ../../../../../i18n/zh_CN/libtodaydatebookplugin.ts \
         ../../../../../i18n/zh_TW/libtodaydatebookplugin.ts \
         ../../../../../i18n/it/libtodaydatebookplugin.ts \
         ../../../../../i18n/da/libtodaydatebookplugin.ts

include ( $(OPIEDIR)/include.pro )
