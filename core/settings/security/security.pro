TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= security.h
SOURCES		= security.cpp main.cpp
INTERFACES	= securitybase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= security

TRANSLATIONS   = ../../i18n/de/security.ts 
TRANSLATIONS   += ../../i18n/pt_BR/security.ts
