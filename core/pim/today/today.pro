CONFIG		+= qt warn on release quick-app

HEADERS		= today.h todaybase.h todayconfig.h
SOURCES		= today.cpp todaybase.cpp todayconfig.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	=
TARGET		= today

TRANSLATIONS = ../../../i18n/de/today.ts \
	 ../../../i18n/nl/today.ts \
	 ../../../i18n/xx/today.ts \
	 ../../../i18n/en/today.ts \
	 ../../../i18n/es/today.ts \
	 ../../../i18n/fr/today.ts \
	 ../../../i18n/hu/today.ts \
	 ../../../i18n/ja/today.ts \
	 ../../../i18n/ko/today.ts \
	 ../../../i18n/no/today.ts \
	 ../../../i18n/pl/today.ts \
	 ../../../i18n/pt/today.ts \
	 ../../../i18n/pt_BR/today.ts \
	 ../../../i18n/sl/today.ts \
	 ../../../i18n/zh_CN/today.ts \
	 ../../../i18n/zh_TW/today.ts \
   	 ../../../i18n/it/today.ts \
   	 ../../../i18n/da/today.ts

include ( $(OPIEDIR)/include.pro )
