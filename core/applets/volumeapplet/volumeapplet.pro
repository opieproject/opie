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

TRANSLATIONS = ../../../i18n/de/libvolumeapplet.ts \
	 ../../../i18n/en/libvolumeapplet.ts \
	 ../../../i18n/es/libvolumeapplet.ts \
	 ../../../i18n/fr/libvolumeapplet.ts \
	 ../../../i18n/hu/libvolumeapplet.ts \
	 ../../../i18n/ja/libvolumeapplet.ts \
	 ../../../i18n/ko/libvolumeapplet.ts \
	 ../../../i18n/no/libvolumeapplet.ts \
	 ../../../i18n/pl/libvolumeapplet.ts \
	 ../../../i18n/pt/libvolumeapplet.ts \
	 ../../../i18n/pt_BR/libvolumeapplet.ts \
	 ../../../i18n/sl/libvolumeapplet.ts \
	 ../../../i18n/zh_CN/libvolumeapplet.ts \
	 ../../../i18n/zh_TW/libvolumeapplet.ts
