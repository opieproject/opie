TEMPLATE    =  lib
CONFIG       = qt embedded release warn_on
SOURCES     =  liquid.cpp \
               liquiddeco.cpp \
               effects.cpp \
               liquidset.cpp \
               plugin.cpp
               
HEADERS     =  liquid.h \
               liquiddeco.h \
               effects.h \
               liquidset.h \
               plugin.h
               
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = liquid
VERSION      = 1.0.0

TRANSLATIONS = ../../../i18n/de/libliquid.ts \
	 ../../../i18n/en/libliquid.ts \
	 ../../../i18n/es/libliquid.ts \
	 ../../../i18n/fr/libliquid.ts \
	 ../../../i18n/hu/libliquid.ts \
	 ../../../i18n/ja/libliquid.ts \
	 ../../../i18n/ko/libliquid.ts \
	 ../../../i18n/no/libliquid.ts \
	 ../../../i18n/pl/libliquid.ts \
	 ../../../i18n/pt/libliquid.ts \
	 ../../../i18n/pt_BR/libliquid.ts \
	 ../../../i18n/sl/libliquid.ts \
	 ../../../i18n/zh_CN/libliquid.ts \
	 ../../../i18n/zh_TW/libliquid.ts
