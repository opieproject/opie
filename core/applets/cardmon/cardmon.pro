TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=	cardmon.h cardmonimpl.h
SOURCES	=	cardmon.cpp cardmonimpl.cpp
TARGET		= cardmonapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include ../launcher
LIBS            += -lqpe -lopie
VERSION		= 1.0.0

TRANSLATIONS = ../../../i18n/de/libcardmonapplet.ts \
	 ../../../i18n/da/libcardmonapplet.ts \
	 ../../../i18n/xx/libcardmonapplet.ts \
	 ../../../i18n/en/libcardmonapplet.ts \
	 ../../../i18n/es/libcardmonapplet.ts \
	 ../../../i18n/fr/libcardmonapplet.ts \
	 ../../../i18n/hu/libcardmonapplet.ts \
	 ../../../i18n/ja/libcardmonapplet.ts \
	 ../../../i18n/ko/libcardmonapplet.ts \
	 ../../../i18n/no/libcardmonapplet.ts \
	 ../../../i18n/pl/libcardmonapplet.ts \
	 ../../../i18n/pt/libcardmonapplet.ts \
	 ../../../i18n/pt_BR/libcardmonapplet.ts \
	 ../../../i18n/sl/libcardmonapplet.ts \
	 ../../../i18n/zh_CN/libcardmonapplet.ts \
	 ../../../i18n/zh_TW/libcardmonapplet.ts
