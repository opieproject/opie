# $Id: citytime.pro,v 1.10 2002-06-26 23:13:52 sandman Exp $
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

TRANSLATIONS = ../../../i18n/de/citytime.ts \
	 ../../../i18n/en/citytime.ts \
	 ../../../i18n/es/citytime.ts \
	 ../../../i18n/fr/citytime.ts \
	 ../../../i18n/hu/citytime.ts \
	 ../../../i18n/ja/citytime.ts \
	 ../../../i18n/ko/citytime.ts \
	 ../../../i18n/no/citytime.ts \
	 ../../../i18n/pl/citytime.ts \
	 ../../../i18n/pt/citytime.ts \
	 ../../../i18n/pt_BR/citytime.ts \
	 ../../../i18n/sl/citytime.ts \
	 ../../../i18n/zh_CN/citytime.ts \
	 ../../../i18n/zh_TW/citytime.ts
