TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   clock.h clockappletimpl.h
SOURCES	=   clock.cpp clockappletimpl.cpp
TARGET		= clockapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../../i18n/de/libclockapplet.ts \
	 ../../../i18n/da/libclockapplet.ts \
	 ../../../i18n/xx/libclockapplet.ts \
	 ../../../i18n/en/libclockapplet.ts \
	 ../../../i18n/es/libclockapplet.ts \
	 ../../../i18n/fr/libclockapplet.ts \
	 ../../../i18n/hu/libclockapplet.ts \
	 ../../../i18n/ja/libclockapplet.ts \
	 ../../../i18n/ko/libclockapplet.ts \
	 ../../../i18n/no/libclockapplet.ts \
	 ../../../i18n/pl/libclockapplet.ts \
	 ../../../i18n/pt/libclockapplet.ts \
	 ../../../i18n/pt_BR/libclockapplet.ts \
	 ../../../i18n/sl/libclockapplet.ts \
	 ../../../i18n/zh_CN/libclockapplet.ts \
	 ../../../i18n/zh_TW/libclockapplet.ts
