TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   battery.h batterystatus.h batteryappletimpl.h
SOURCES	=   battery.cpp batterystatus.cpp batteryappletimpl.cpp
TARGET		= batteryapplet
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS += ../../i18n/de/libbatteryapplet.ts
