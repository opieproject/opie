TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= checkers.h \
		  echeckers.h \
		  field.h \
		  kcheckers.h \
		  rcheckers.h
SOURCES		= checkers.cpp \
		  echeckers.cpp \
		  field.cpp \
		  kcheckers.cpp \
		  main.cpp \
		  rcheckers.cpp
INTERFACES	= 
INCLUDEPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR		= $(OPIEDIR)/bin
TARGET		= kcheckers

TRANSLATIONS = ../../../i18n/de/kcheckers.ts \
	 ../../../i18n/en/kcheckers.ts \
	 ../../../i18n/es/kcheckers.ts \
	 ../../../i18n/fr/kcheckers.ts \
	 ../../../i18n/hu/kcheckers.ts \
	 ../../../i18n/ja/kcheckers.ts \
	 ../../../i18n/ko/kcheckers.ts \
	 ../../../i18n/no/kcheckers.ts \
	 ../../../i18n/pl/kcheckers.ts \
	 ../../../i18n/pt/kcheckers.ts \
	 ../../../i18n/pt_BR/kcheckers.ts \
	 ../../../i18n/sl/kcheckers.ts \
	 ../../../i18n/zh_CN/kcheckers.ts \
	 ../../../i18n/zh_TW/kcheckers.ts
