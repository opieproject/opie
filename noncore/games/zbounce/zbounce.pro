TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= game.h kbounce.h 
SOURCES		= game.cpp kbounce.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe
DESTDIR     = $(OPIEDIR)/bin
TARGET		= zbounce 

TRANSLATIONS = ../../../i18n/de/zbounce.ts \
	 ../../../i18n/en/zbounce.ts \
	 ../../../i18n/es/zbounce.ts \
	 ../../../i18n/fr/zbounce.ts \
	 ../../../i18n/hu/zbounce.ts \
	 ../../../i18n/ja/zbounce.ts \
	 ../../../i18n/ko/zbounce.ts \
	 ../../../i18n/no/zbounce.ts \
	 ../../../i18n/pl/zbounce.ts \
	 ../../../i18n/pt/zbounce.ts \
	 ../../../i18n/pt_BR/zbounce.ts \
	 ../../../i18n/sl/zbounce.ts \
	 ../../../i18n/zh_CN/zbounce.ts \
	 ../../../i18n/zh_TW/zbounce.ts \
   	 ../../../i18n/it/zbounce.ts
