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

TRANSLATIONS = ../../../i18n/de/mediummount.ts \
	 ../../../i18n/da/mediummount.ts \
	 ../../../i18n/xx/mediummount.ts \
	 ../../../i18n/en/mediummount.ts \
	 ../../../i18n/es/mediummount.ts \
	 ../../../i18n/fr/mediummount.ts \
	 ../../../i18n/hu/mediummount.ts \
	 ../../../i18n/ja/mediummount.ts \
	 ../../../i18n/ko/mediummount.ts \
	 ../../../i18n/no/mediummount.ts \
	 ../../../i18n/pl/mediummount.ts \
	 ../../../i18n/pt/mediummount.ts \
	 ../../../i18n/pt_BR/mediummount.ts \
	 ../../../i18n/sl/mediummount.ts \
	 ../../../i18n/zh_CN/mediummount.ts \
	 ../../../i18n/zh_TW/mediummount.ts
