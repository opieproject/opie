TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   volume.h volumeappletimpl.h oledbox.h
SOURCES	=   volume.cpp volumeappletimpl.cpp oledbox.cpp
TARGET		= volumeapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libvolumeapplet.ts
TRANSLATIONS += ../../i18n/es/libvolumeapplet.ts
TRANSLATIONS += ../../i18n/pt/libvolumeapplet.ts
TRANSLATIONS += ../../i18n/pt_BR/libvolumeapplet.ts
