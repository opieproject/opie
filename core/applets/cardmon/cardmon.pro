TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=	cardmon.h cardmonimpl.h
SOURCES	=	cardmon.cpp cardmonimpl.cpp
TARGET		= cardmonapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include ../launcher
LIBS            += -lqpe
VERSION		= 1.0.0



TRANSLATIONS = ../../../i18n/de/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/en/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/es/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/fr/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/hu/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/ja/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/ko/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/no/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/pl/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/pt/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/pt_BR/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/sl/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/zh_CN/libcardmonapplet.ts
TRANSLATIONS += ../../../i18n/zh_TW/libcardmonapplet.ts

