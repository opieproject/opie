TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= example.h
SOURCES		= main.cpp example.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= examplebase.ui
TARGET		= example

TRANSLATIONS = ../i18n/de/example.ts \
	 ../i18n/da/example.ts \
	 ../i18n/xx/example.ts \
	 ../i18n/en/example.ts \
	 ../i18n/es/example.ts \
	 ../i18n/fr/example.ts \
	 ../i18n/hu/example.ts \
	 ../i18n/ja/example.ts \
	 ../i18n/ko/example.ts \
	 ../i18n/no/example.ts \
	 ../i18n/pl/example.ts \
	 ../i18n/pt/example.ts \
	 ../i18n/pt_BR/example.ts \
	 ../i18n/sl/example.ts \
	 ../i18n/zh_CN/example.ts \
	 ../i18n/zh_TW/example.ts
