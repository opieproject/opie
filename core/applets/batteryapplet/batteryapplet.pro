TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   battery.h batterystatus.h batteryappletimpl.h
SOURCES	=   battery.cpp batterystatus.cpp batteryappletimpl.cpp
TARGET		= batteryapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../../i18n/de/libbatteryapplet.ts \
	 ../../../i18n/da/libbatteryapplet.ts \
	 ../../../i18n/xx/libbatteryapplet.ts \
	 ../../../i18n/en/libbatteryapplet.ts \
	 ../../../i18n/es/libbatteryapplet.ts \
	 ../../../i18n/fr/libbatteryapplet.ts \
	 ../../../i18n/hu/libbatteryapplet.ts \
	 ../../../i18n/ja/libbatteryapplet.ts \
	 ../../../i18n/ko/libbatteryapplet.ts \
	 ../../../i18n/no/libbatteryapplet.ts \
	 ../../../i18n/pl/libbatteryapplet.ts \
	 ../../../i18n/pt/libbatteryapplet.ts \
	 ../../../i18n/pt_BR/libbatteryapplet.ts \
	 ../../../i18n/sl/libbatteryapplet.ts \
	 ../../../i18n/zh_CN/libbatteryapplet.ts \
	 ../../../i18n/zh_TW/libbatteryapplet.ts
