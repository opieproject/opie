TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   clock.h clockappletimpl.h
SOURCES	=   clock.cpp clockappletimpl.cpp
TARGET		= clockapplet
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libclockapplet.ts
TRANSLATIONS += ../../i18n/es/libclockapplet.ts
TRANSLATIONS += ../../i18n/pt/libclockapplet.ts
TRANSLATIONS += ../../i18n/pt_BR/libclockapplet.ts
