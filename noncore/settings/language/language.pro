TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= settings.h
SOURCES		= language.cpp main.cpp
INTERFACES	= languagesettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= language

TRANSLATIONS = ../../../i18n/de/language.ts \
	 ../../../i18n/en/language.ts \
	 ../../../i18n/es/language.ts \
	 ../../../i18n/fr/language.ts \
	 ../../../i18n/hu/language.ts \
	 ../../../i18n/ja/language.ts \
	 ../../../i18n/ko/language.ts \
	 ../../../i18n/no/language.ts \
	 ../../../i18n/pl/language.ts \
	 ../../../i18n/pt/language.ts \
	 ../../../i18n/pt_BR/language.ts \
	 ../../../i18n/sl/language.ts \
	 ../../../i18n/zh_CN/language.ts \
	 ../../../i18n/zh_TW/language.ts
