TEMPLATE    =  lib
CONFIG       = qt embedded release warn_on
SOURCES     =  metal.cpp plugin.cpp
               
HEADERS     =  metal.h plugin.h
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = metal
VERSION      = 1.0.0

TRANSLATIONS = ../../../i18n/de/libmetal.ts \
	 ../../../i18n/en/libmetal.ts \
	 ../../../i18n/es/libmetal.ts \
	 ../../../i18n/fr/libmetal.ts \
	 ../../../i18n/hu/libmetal.ts \
	 ../../../i18n/ja/libmetal.ts \
	 ../../../i18n/ko/libmetal.ts \
	 ../../../i18n/no/libmetal.ts \
	 ../../../i18n/pl/libmetal.ts \
	 ../../../i18n/pt/libmetal.ts \
	 ../../../i18n/pt_BR/libmetal.ts \
	 ../../../i18n/sl/libmetal.ts \
	 ../../../i18n/zh_CN/libmetal.ts \
	 ../../../i18n/zh_TW/libmetal.ts
