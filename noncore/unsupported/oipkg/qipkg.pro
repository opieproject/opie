DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= packagemanager.h
SOURCES		= packagemanager.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= packagemanagerbase.ui pkdesc.ui pkfind.ui pksettings.ui
TARGET		= qipkg

TRANSLATIONS = ../i18n/de/qipkg.ts
TRANSLATIONS += ../i18n/fr/qipkg.ts
TRANSLATIONS += ../i18n/pt_BR/qipkg.ts
