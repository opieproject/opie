TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= helpbrowser.h
SOURCES		= helpbrowser.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../i18n/de/helpbrowser.ts
TRANSLATIONS = ../i18n/pt_BR/helpbrowser.ts
