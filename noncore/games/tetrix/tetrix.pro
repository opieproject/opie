TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= ../bin
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

TRANSLATIONS = ../i18n/pt_BR/tetrix.ts
TRANSLATIONS += ../i18n/es/tetrix.ts
TRANSLATIONS += ../i18n/pt/tetrix.ts
TRANSLATIONS   += ../i18n/de/tetrix.ts
TRANSLATIONS   += ../i18n/en/tetrix.ts
TRANSLATIONS   += ../i18n/fr/tetrix.ts
TRANSLATIONS   += ../i18n/hu/tetrix.ts
TRANSLATIONS   += ../i18n/sl/tetrix.ts
TRANSLATIONS   += ../i18n/ja/tetrix.ts
TRANSLATIONS   += ../i18n/ko/tetrix.ts
TRANSLATIONS   += ../i18n/no/tetrix.ts
TRANSLATIONS   += ../i18n/zh_CN/tetrix.ts
TRANSLATIONS   += ../i18n/pl/tetrix.ts
TRANSLATIONS   += ../i18n/zh_TW/tetrix.ts
