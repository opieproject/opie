# $Id: citytime.pro,v 1.1 2002-01-25 22:14:27 kergoth Exp $
CONFIG += qt warn_on release
TEMPLATE = app
DESTDIR         = $(QPEDIR)/bin
INTERFACES = citytimebase.ui
HEADERS = citytime.h zonemap.h sun.h stylusnormalizer.h
SOURCES = citytime.cpp zonemap.cpp main.cpp sun.c stylusnormalizer.cpp
TARGET = citytime
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += $(QPEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/citytime.ts
