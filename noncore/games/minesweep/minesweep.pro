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

TRANSLATIONS = ../../../i18n/de/minesweep.pro.in.ts \
	 ../../../i18n/en/minesweep.pro.in.ts \
	 ../../../i18n/es/minesweep.pro.in.ts \
	 ../../../i18n/fr/minesweep.pro.in.ts \
	 ../../../i18n/hu/minesweep.pro.in.ts \
	 ../../../i18n/ja/minesweep.pro.in.ts \
	 ../../../i18n/ko/minesweep.pro.in.ts \
	 ../../../i18n/no/minesweep.pro.in.ts \
	 ../../../i18n/pl/minesweep.pro.in.ts \
	 ../../../i18n/pt/minesweep.pro.in.ts \
	 ../../../i18n/pt_BR/minesweep.pro.in.ts \
	 ../../../i18n/sl/minesweep.pro.in.ts \
	 ../../../i18n/zh_CN/minesweep.pro.in.ts \
	 ../../../i18n/zh_TW/minesweep.pro.in.ts
