SOURCES	+= chess.cpp main.cpp 
HEADERS	+= chess.h 
DESTDIR = $(OPIEDIR)/bin
TARGET = chess
DEPENDPATH	+= $(OPIEDIR)/include
INTERFACES = mainwindow.ui 
IMAGES	= images/new.png images/repeat.png images/txt.png images/back.png 
TEMPLATE	=app
CONFIG	+= qt warn_on release
INCLUDEPATH += $(OPIEDIR)/include
LIBS	+= -lqpe
DBFILE	= chess.db
LANGUAGE	= C++
CPP_ALWAYS_CREATE_SOURCE	= TRUE

TRANSLATIONS = ../../../i18n/de/chess.ts \
	 ../../../i18n/da/chess.ts \
	 ../../../i18n/xx/chess.ts \
	 ../../../i18n/en/chess.ts \
	 ../../../i18n/es/chess.ts \
	 ../../../i18n/fr/chess.ts \
	 ../../../i18n/hu/chess.ts \
	 ../../../i18n/ja/chess.ts \
	 ../../../i18n/ko/chess.ts \
	 ../../../i18n/no/chess.ts \
	 ../../../i18n/pl/chess.ts \
	 ../../../i18n/pt/chess.ts \
	 ../../../i18n/pt_BR/chess.ts \
	 ../../../i18n/sl/chess.ts \
	 ../../../i18n/zh_CN/chess.ts \
	 ../../../i18n/zh_TW/chess.ts
