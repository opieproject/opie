TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../bin

HEADERS	= dicttool.h
SOURCES	= dicttool.cpp main.cpp

INTERFACES	= dicttoolbase.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TARGET		= dictionary

TRANSLATIONS    = ../i18n/pt_BR/dictionary.ts
TRANSLATIONS   += ../i18n/de/dictionary.ts
TRANSLATIONS   += ../i18n/en/dictionary.ts
TRANSLATIONS   += ../i18n/hu/dictionary.ts
TRANSLATIONS   += ../i18n/sl/dictionary.ts
TRANSLATIONS   += ../i18n/pl/dictionary.ts
TRANSLATIONS   += ../i18n/fr/dictionary.ts
TRANSLATIONS   += ../i18n/ja/dictionary.ts
TRANSLATIONS   += ../i18n/ko/dictionary.ts
TRANSLATIONS   += ../i18n/no/dictionary.ts
TRANSLATIONS   += ../i18n/zh_CN/dictionary.ts
TRANSLATIONS   += ../i18n/zh_TW/dictionary.ts
TRANSLATIONS   += ../i18n/es/dictionary.ts
