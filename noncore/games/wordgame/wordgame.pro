TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= wordgame.h
SOURCES		= main.cpp \
		  wordgame.cpp
INTERFACES	= newgamebase.ui rulesbase.ui
TARGET		= wordgame
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/wordgame.ts
TRANSLATIONS += ../i18n/pt_BR/wordgame.ts
TRANSLATIONS   += ../i18n/en/wordgame.ts
TRANSLATIONS   += ../i18n/hu/wordgame.ts
TRANSLATIONS   += ../i18n/ja/wordgame.ts
TRANSLATIONS   += ../i18n/ko/wordgame.ts
TRANSLATIONS   += ../i18n/no/wordgame.ts
TRANSLATIONS   += ../i18n/zh_CN/wordgame.ts
TRANSLATIONS   += ../i18n/zh_TW/wordgame.ts
