TEMPLATE     = lib
CONFIG      += qt warn_on release
HEADERS      = wireless.h wirelessappletimpl.h networkinfo.h mgraph.h
SOURCES      = wireless.cpp wirelessappletimpl.cpp networkinfo.cpp mgraph.cpp
TARGET       = wirelessapplet
DESTDIR      = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += ../$(OPIEDIR)/include
LIBS        += -lqpe
VERSION      = 0.0.3

TRANSLATIONS = ../../../i18n/de/libwirelessapplet.ts \
	 ../../../i18n/en/libwirelessapplet.ts \
	 ../../../i18n/es/libwirelessapplet.ts \
	 ../../../i18n/fr/libwirelessapplet.ts \
	 ../../../i18n/hu/libwirelessapplet.ts \
	 ../../../i18n/ja/libwirelessapplet.ts \
	 ../../../i18n/ko/libwirelessapplet.ts \
	 ../../../i18n/no/libwirelessapplet.ts \
	 ../../../i18n/pl/libwirelessapplet.ts \
	 ../../../i18n/pt/libwirelessapplet.ts \
	 ../../../i18n/pt_BR/libwirelessapplet.ts \
	 ../../../i18n/sl/libwirelessapplet.ts \
	 ../../../i18n/zh_CN/libwirelessapplet.ts \
	 ../../../i18n/zh_TW/libwirelessapplet.ts
