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

TRANSLATIONS = ../../../i18n/de/wordgame.ts \
	 ../../../i18n/en/wordgame.ts \
	 ../../../i18n/es/wordgame.ts \
	 ../../../i18n/fr/wordgame.ts \
	 ../../../i18n/hu/wordgame.ts \
	 ../../../i18n/ja/wordgame.ts \
	 ../../../i18n/ko/wordgame.ts \
	 ../../../i18n/no/wordgame.ts \
	 ../../../i18n/pl/wordgame.ts \
	 ../../../i18n/pt/wordgame.ts \
	 ../../../i18n/pt_BR/wordgame.ts \
	 ../../../i18n/sl/wordgame.ts \
	 ../../../i18n/zh_CN/wordgame.ts \
	 ../../../i18n/zh_TW/wordgame.ts
