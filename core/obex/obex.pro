TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   obex.h obeximpl.h obexhandler.h obexsend.h receiver.h
SOURCES	=   obex.cc obeximpl.cpp obexsend.cpp obexhandler.cpp receiver.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopie
VERSION		= 0.0.2

TRANSLATIONS = ../../../i18n/de/libopieobex.ts \
	 ../../../i18n/da/libopieobex.ts \
	 ../../../i18n/xx/libopieobex.ts \
	 ../../../i18n/en/libopieobex.ts \
	 ../../../i18n/es/libopieobex.ts \
	 ../../../i18n/fr/libopieobex.ts \
	 ../../../i18n/hu/libopieobex.ts \
	 ../../../i18n/ja/libopieobex.ts \
	 ../../../i18n/ko/libopieobex.ts \
	 ../../../i18n/no/libopieobex.ts \
	 ../../../i18n/pl/libopieobex.ts \
	 ../../../i18n/pt/libopieobex.ts \
	 ../../../i18n/pt_BR/libopieobex.ts \
	 ../../../i18n/sl/libopieobex.ts \
	 ../../../i18n/zh_CN/libopieobex.ts \
	 ../../../i18n/zh_TW/libopieobex.ts



include ( $(OPIEDIR)/include.pro )
