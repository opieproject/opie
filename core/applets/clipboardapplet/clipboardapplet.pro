TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   clipboard.h clipboardappletimpl.h
SOURCES	=   clipboard.cpp clipboardappletimpl.cpp
TARGET		= clipboardapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../../i18n/de/libclipboardapplet.ts \
	 ../../../i18n/da/libclipboardapplet.ts \
	 ../../../i18n/xx/libclipboardapplet.ts \
	 ../../../i18n/en/libclipboardapplet.ts \
	 ../../../i18n/es/libclipboardapplet.ts \
	 ../../../i18n/fr/libclipboardapplet.ts \
	 ../../../i18n/hu/libclipboardapplet.ts \
	 ../../../i18n/ja/libclipboardapplet.ts \
	 ../../../i18n/ko/libclipboardapplet.ts \
	 ../../../i18n/no/libclipboardapplet.ts \
	 ../../../i18n/pl/libclipboardapplet.ts \
	 ../../../i18n/pt/libclipboardapplet.ts \
	 ../../../i18n/pt_BR/libclipboardapplet.ts \
	 ../../../i18n/sl/libclipboardapplet.ts \
	 ../../../i18n/zh_CN/libclipboardapplet.ts \
	 ../../../i18n/zh_TW/libclipboardapplet.ts
