TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   clock.h clockappletimpl.h
SOURCES	=   clock.cpp clockappletimpl.cpp
TARGET		= clockapplet
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += ../$(QPEDIR)/include ..
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS += ../../i18n/de/libclockapplet.ts
