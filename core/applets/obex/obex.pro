TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   obex.h obeximpl.h
SOURCES	=   obex.cc obeximpl.cc
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher
DEPENDPATH      += ../$(OPIEDIR)/include
INTERFACES      = obexinc.ui obexdlg.ui
LIBS            += -lqpe -lopie
VERSION		= 0.0.1

TRANSLATIONS = ../../../i18n/de/libopieobex.ts \
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
