TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= mobilemsg.h
SOURCES		= main.cpp mobilemsg.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= mobilemsg
DESTDIR		= $(OPIEDIR)/bin
INTERFACES	= mobilemsgbase.ui

TRANSLATIONS = ../../../i18n/de/mobilemsg.ts \
	 ../../../i18n/da/mobilemsg.ts \
	 ../../../i18n/xx/mobilemsg.ts \
	 ../../../i18n/en/mobilemsg.ts \
	 ../../../i18n/es/mobilemsg.ts \
	 ../../../i18n/fr/mobilemsg.ts \
	 ../../../i18n/hu/mobilemsg.ts \
	 ../../../i18n/ja/mobilemsg.ts \
	 ../../../i18n/ko/mobilemsg.ts \
	 ../../../i18n/no/mobilemsg.ts \
	 ../../../i18n/pl/mobilemsg.ts \
	 ../../../i18n/pt/mobilemsg.ts \
	 ../../../i18n/pt_BR/mobilemsg.ts \
	 ../../../i18n/sl/mobilemsg.ts \
	 ../../../i18n/zh_CN/mobilemsg.ts \
	 ../../../i18n/zh_TW/mobilemsg.ts
