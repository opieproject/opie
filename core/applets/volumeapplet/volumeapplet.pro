TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   volume.h volumeappletimpl.h
SOURCES	=   volume.cpp volumeappletimpl.cpp
TARGET		= volumeapplet
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += ../$(QPEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS += ../../i18n/de/libvolumeapplet.ts
