TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=	cardmon.h cardmonimpl.h
SOURCES	=	cardmon.cpp cardmonimpl.cpp
TARGET		= cardmonapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include ../taskbar
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../i18n/de/libcardmonapplet.ts
