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

TRANSLATIONS    = ../i18n/pt_BR/qipkg.ts
TRANSLATIONS   += ../i18n/de/qipkg.ts
TRANSLATIONS   += ../i18n/en/qipkg.ts
TRANSLATIONS   += ../i18n/hu/qipkg.ts
TRANSLATIONS   += ../i18n/sl/qipkg.ts
TRANSLATIONS   += ../i18n/ja/qipkg.ts
TRANSLATIONS   += ../i18n/ko/qipkg.ts
TRANSLATIONS   += ../i18n/no/qipkg.ts
TRANSLATIONS   += ../i18n/zh_CN/qipkg.ts
TRANSLATIONS   += ../i18n/zh_TW/qipkg.ts
TRANSLATIONS   += ../i18n/fr/qipkg.ts
