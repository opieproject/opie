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
TRANSLATIONS += ../i18n/pt_BR/helpbrowser.ts
TRANSLATIONS   += ../i18n/en/helpbrowser.ts
TRANSLATIONS   += ../i18n/hu/helpbrowser.ts
TRANSLATIONS   += ../i18n/ja/helpbrowser.ts
TRANSLATIONS   += ../i18n/ko/helpbrowser.ts
TRANSLATIONS   += ../i18n/no/helpbrowser.ts
TRANSLATIONS   += ../i18n/fr/helpbrowser.ts
TRANSLATIONS   += ../i18n/zh_CN/helpbrowser.ts
TRANSLATIONS   += ../i18n/zh_TW/helpbrowser.ts
