TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= security.h
SOURCES		= security.cpp main.cpp
INTERFACES	= securitybase.ui
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= ../$(QPEDIR)/include
LIBS            += -lqpe
TARGET		= security

TRANSLATIONS   = ../../i18n/de/security.ts 
