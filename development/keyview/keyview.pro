TEMPLATE	= lib
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= keyview.h keyboardimpl.h
SOURCES		= keyview.cpp keyboardimpl.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
INTERFACES	= 
TARGET		= keyview
DESTDIR     = $(OPIEDIR)/plugins/inputmethods

TRANSLATIONS = ../i18n/de/keyview.ts \
	 ../i18n/da/keyview.ts \
	 ../i18n/xx/keyview.ts \
	 ../i18n/en/keyview.ts \
	 ../i18n/es/keyview.ts \
	 ../i18n/fr/keyview.ts \
	 ../i18n/hu/keyview.ts \
	 ../i18n/ja/keyview.ts \
	 ../i18n/ko/keyview.ts \
	 ../i18n/no/keyview.ts \
	 ../i18n/pl/keyview.ts \
	 ../i18n/pt/keyview.ts \
	 ../i18n/pt_BR/keyview.ts \
	 ../i18n/sl/keyview.ts \
	 ../i18n/zh_CN/keyview.ts \
	 ../i18n/zh_TW/keyview.ts
