TEMPLATE	= app
DESTDIR         = $(OPIEDIR)/bin/
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= mediumwidget.h mediumglobal.h mainwindow.h
SOURCES		= main.cpp mediumwidget.cc mediumglobal.cc mainwindow.cc
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET          = mediummount


TRANSLATIONS = ../../../i18n/de/mediummount.ts
TRANSLATIONS += ../../../i18n/en/mediummount.ts
TRANSLATIONS += ../../../i18n/es/mediummount.ts
TRANSLATIONS += ../../../i18n/fr/mediummount.ts
TRANSLATIONS += ../../../i18n/hu/mediummount.ts
TRANSLATIONS += ../../../i18n/ja/mediummount.ts
TRANSLATIONS += ../../../i18n/ko/mediummount.ts
TRANSLATIONS += ../../../i18n/no/mediummount.ts
TRANSLATIONS += ../../../i18n/pl/mediummount.ts
TRANSLATIONS += ../../../i18n/pt/mediummount.ts
TRANSLATIONS += ../../../i18n/pt_BR/mediummount.ts
TRANSLATIONS += ../../../i18n/sl/mediummount.ts
TRANSLATIONS += ../../../i18n/zh_CN/mediummount.ts
TRANSLATIONS += ../../../i18n/zh_TW/mediummount.ts

