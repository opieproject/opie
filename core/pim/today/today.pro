TEMPLATE = app
#CONFIG		= qt warn_on 
CONFIG -= moc
CONFIG		+= qt release

HEADERS		= today.h todaybase.h todayconfig.h
SOURCES		= today.cpp todaybase.cpp todayconfig.cpp
INTERFACES = todayconfigmiscbase.ui

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

oapp {
	TEMPLATE	= lib
	SOURCES		+= plugin.cpp
	LIBS		+= -loapp

	DESTDIR		= $(OPIEDIR)/plugins/app
	INCLUDEPATH	+= $(OPIEDIR)/core/apps/oapp
	DEPENDPATH	+= $(OPIEDIR)/core/apps/oapp
}
else {
	TEMPLATE	= app
	SOURCES		+= main.cpp
	DESTDIR		= $(OPIEDIR)/bin
}
