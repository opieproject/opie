TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= gtetrix.h \
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
TARGET		= tetrix

TRANSLATIONS = ../../../i18n/de/tetrix.ts \
	 ../../../i18n/da/tetrix.ts \
	 ../../../i18n/xx/tetrix.ts \
	 ../../../i18n/en/tetrix.ts \
	 ../../../i18n/es/tetrix.ts \
	 ../../../i18n/fr/tetrix.ts \
	 ../../../i18n/hu/tetrix.ts \
	 ../../../i18n/ja/tetrix.ts \
	 ../../../i18n/ko/tetrix.ts \
	 ../../../i18n/no/tetrix.ts \
	 ../../../i18n/pl/tetrix.ts \
	 ../../../i18n/pt/tetrix.ts \
	 ../../../i18n/pt_BR/tetrix.ts \
	 ../../../i18n/sl/tetrix.ts \
	 ../../../i18n/zh_CN/tetrix.ts \
	 ../../../i18n/it/tetrix.ts \
	 ../../../i18n/zh_TW/tetrix.ts
