# $Id: citytime.pro,v 1.8 2002-04-13 13:20:52 cniehaus Exp $
CONFIG += qt warn_on release
TEMPLATE = app
DESTDIR         = $(OPIEDIR)/bin
INTERFACES = citytimebase.ui
HEADERS = citytime.h zonemap.h sun.h stylusnormalizer.h
SOURCES = citytime.cpp zonemap.cpp main.cpp sun.c stylusnormalizer.cpp
TARGET = citytime
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/citytime.ts
TRANSLATIONS += ../i18n/pt_BR/citytime.ts
TRANSLATIONS   += ../i18n/en/citytime.ts
TRANSLATIONS   += ../i18n/hu/citytime.ts
TRANSLATIONS   += ../i18n/ja/citytime.ts
TRANSLATIONS   += ../i18n/ko/citytime.ts
TRANSLATIONS   += ../i18n/pl/citytime.ts
TRANSLATIONS   += ../i18n/fr/citytime.ts
TRANSLATIONS   += ../i18n/sl/citytime.ts
TRANSLATIONS   += ../i18n/no/citytime.ts
TRANSLATIONS   += ../i18n/zh_CN/citytime.ts
TRANSLATIONS   += ../i18n/zh_TW/citytime.ts
