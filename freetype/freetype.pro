TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=	fontfactoryttf_qws.h freetypefactoryimpl.h
SOURCES	=	fontfactoryttf_qws.cpp freetypefactoryimpl.cpp
qt2:HEADERS	+= qfontdata_p.h
TARGET		= freetypefactory
DESTDIR		= ../plugins/fontfactories
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
#INCLUDEPATH += $(OPIEDIR)/include
#DEPENDPATH	+= /usr/local/include/freetype2
LIBS            += -lqpe -lfreetype
VERSION		= 1.0.0

TRANSLATIONS = ../i18n/de/libfreetypefactory.ts \
	 ../i18n/en/libfreetypefactory.ts \
	 ../i18n/es/libfreetypefactory.ts \
	 ../i18n/fr/libfreetypefactory.ts \
	 ../i18n/hu/libfreetypefactory.ts \
	 ../i18n/ja/libfreetypefactory.ts \
	 ../i18n/ko/libfreetypefactory.ts \
	 ../i18n/no/libfreetypefactory.ts \
	 ../i18n/pl/libfreetypefactory.ts \
	 ../i18n/pt/libfreetypefactory.ts \
	 ../i18n/pt_BR/libfreetypefactory.ts \
	 ../i18n/sl/libfreetypefactory.ts \
	 ../i18n/zh_CN/libfreetypefactory.ts \
	 ../i18n/zh_TW/libfreetypefactory.ts
