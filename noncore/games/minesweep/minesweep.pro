TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= minefield.h \
		  minesweep.h
SOURCES		= main.cpp \
		  minefield.cpp \
		  minesweep.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../i18n/de/minesweep.ts
TRANSLATIONS += ../i18n/pt_BR/minesweep.ts
TRANSLATIONS   += ../i18n/en/minesweep.ts
TRANSLATIONS   += ../i18n/hu/minesweep.ts
TRANSLATIONS   += ../i18n/ja/minesweep.ts
TRANSLATIONS   += ../i18n/fr/minesweep.ts
TRANSLATIONS   += ../i18n/ko/minesweep.ts
TRANSLATIONS   += ../i18n/no/minesweep.ts
TRANSLATIONS   += ../i18n/zh_CN/minesweep.ts
TRANSLATIONS   += ../i18n/zh_TW/minesweep.ts
