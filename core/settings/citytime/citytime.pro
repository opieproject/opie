# $Id: citytime.pro,v 1.4 2002-03-19 02:38:30 kergoth Exp $
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
