TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   vmemo.h vmemoimpl.h
SOURCES	=   vmemo.cpp vmemoimpl.cpp
TARGET		= vmemoapplet
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/pt_BR/libvmemoapplet.ts
