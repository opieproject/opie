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
TARGET		= minesweep

TRANSLATIONS = ../../../i18n/de/minesweep.ts \
	 ../../../i18n/da/minesweep.ts \
	 ../../../i18n/xx/minesweep.ts \
	 ../../../i18n/en/minesweep.ts \
	 ../../../i18n/es/minesweep.ts \
	 ../../../i18n/fr/minesweep.ts \
	 ../../../i18n/hu/minesweep.ts \
	 ../../../i18n/ja/minesweep.ts \
	 ../../../i18n/ko/minesweep.ts \
	 ../../../i18n/no/minesweep.ts \
	 ../../../i18n/pl/minesweep.ts \
	 ../../../i18n/pt/minesweep.ts \
	 ../../../i18n/pt_BR/minesweep.ts \
	 ../../../i18n/sl/minesweep.ts \
	 ../../../i18n/zh_CN/minesweep.ts \
	 ../../../i18n/it/minesweep.ts \
	 ../../../i18n/zh_TW/minesweep.ts
