TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= buzzword.h 
SOURCES		= buzzword.cpp main.cpp
INCLUDEPATH	+= $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
DESTDIR     = $(OPIEDIR)/bin
LIBS        += -lqpe
TARGET		= buzzword 

TRANSLATIONS = ../../../i18n/de/buzzword.ts \
	 ../../../i18n/en/buzzword.ts \
	 ../../../i18n/es/buzzword.ts \
	 ../../../i18n/fr/buzzword.ts \
	 ../../../i18n/hu/buzzword.ts \
	 ../../../i18n/ja/buzzword.ts \
	 ../../../i18n/ko/buzzword.ts \
	 ../../../i18n/no/buzzword.ts \
	 ../../../i18n/pl/buzzword.ts \
	 ../../../i18n/pt/buzzword.ts \
	 ../../../i18n/pt_BR/buzzword.ts \
	 ../../../i18n/sl/buzzword.ts \
	 ../../../i18n/zh_CN/buzzword.ts \
	 ../../../i18n/zh_TW/buzzword.ts \
   	 ../../../i18n/it/buzzword.ts
