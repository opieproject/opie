TEMPLATE    =  lib
CONFIG       = qt embedded release warn_on
SOURCES     =  metal.cpp plugin.cpp
               
HEADERS     =  metal.h
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = metal
#TARGET = liquid
VERSION      = 1.0.0

TRANSLATIONS = ../../../i18n/de/libmetal liquid.ts \
	 ../../../i18n/en/libmetal liquid.ts \
	 ../../../i18n/es/libmetal liquid.ts \
	 ../../../i18n/fr/libmetal liquid.ts \
	 ../../../i18n/hu/libmetal liquid.ts \
	 ../../../i18n/ja/libmetal liquid.ts \
	 ../../../i18n/ko/libmetal liquid.ts \
	 ../../../i18n/no/libmetal liquid.ts \
	 ../../../i18n/pl/libmetal liquid.ts \
	 ../../../i18n/pt/libmetal liquid.ts \
	 ../../../i18n/pt_BR/libmetal liquid.ts \
	 ../../../i18n/sl/libmetal liquid.ts \
	 ../../../i18n/zh_CN/libmetal liquid.ts \
	 ../../../i18n/zh_TW/libmetal liquid.ts
