DESTDIR		= $(QPEDIR)/bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= packagemanager.h
SOURCES		= packagemanager.cpp \
		  main.cpp
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe
INTERFACES	= packagemanagerbase.ui pkdesc.ui pkfind.ui pksettings.ui
TARGET		= qipkg

TRANSLATIONS = ../i18n/de/qipkg.ts
