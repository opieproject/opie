TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= dicttool.h
SOURCES	= dicttool.cpp main.cpp
INTERFACES	= dicttoolbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= dictionary

TRANSLATIONS = ../../../i18n/de/dictionary.ts \
	 ../../../i18n/en/dictionary.ts \
	 ../../../i18n/es/dictionary.ts \
	 ../../../i18n/fr/dictionary.ts \
	 ../../../i18n/hu/dictionary.ts \
	 ../../../i18n/ja/dictionary.ts \
	 ../../../i18n/ko/dictionary.ts \
	 ../../../i18n/no/dictionary.ts \
	 ../../../i18n/pl/dictionary.ts \
	 ../../../i18n/pt/dictionary.ts \
	 ../../../i18n/pt_BR/dictionary.ts \
	 ../../../i18n/sl/dictionary.ts \
	 ../../../i18n/zh_CN/dictionary.ts \
	 ../../../i18n/zh_TW/dictionary.ts
