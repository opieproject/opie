TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   clipboard.h clipboardappletimpl.h
SOURCES	=   clipboard.cpp clipboardappletimpl.cpp
TARGET		= clipboardapplet
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/pt_BR/libclipboardapplet.ts
TRANSLATIONS += ../../i18n/es/libclipboardapplet.ts
TRANSLATIONS += ../../i18n/pt/libclipboardapplet.ts
