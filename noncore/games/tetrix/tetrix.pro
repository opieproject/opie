TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= minefield.h \
		    gtetrix.h \
		    qtetrix.h \
		    qtetrixb.h \
		    tpiece.h
SOURCES		= main.cpp \
		    gtetrix.cpp \
		    qtetrix.cpp \
		    qtetrixb.cpp \
		    tpiece.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../../../i18n/de/tetrix.pro.in.ts \
	 ../../../i18n/en/tetrix.pro.in.ts \
	 ../../../i18n/es/tetrix.pro.in.ts \
	 ../../../i18n/fr/tetrix.pro.in.ts \
	 ../../../i18n/hu/tetrix.pro.in.ts \
	 ../../../i18n/ja/tetrix.pro.in.ts \
	 ../../../i18n/ko/tetrix.pro.in.ts \
	 ../../../i18n/no/tetrix.pro.in.ts \
	 ../../../i18n/pl/tetrix.pro.in.ts \
	 ../../../i18n/pt/tetrix.pro.in.ts \
	 ../../../i18n/pt_BR/tetrix.pro.in.ts \
	 ../../../i18n/sl/tetrix.pro.in.ts \
	 ../../../i18n/zh_CN/tetrix.pro.in.ts \
	 ../../../i18n/zh_TW/tetrix.pro.in.ts
