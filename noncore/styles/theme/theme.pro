TEMPLATE    =  lib
CONFIG       = qt embedded debug warn_on
SOURCES     =  ogfxeffect.cpp \
               othemestyle.cpp \
               othemebase.cpp \
               plugin.cpp

HEADERS     = ogfxeffect.h \
              othemebase.h \
              othemestyle.h

               
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = theme
VERSION      = 1.0.0

TRANSLATIONS = ../../../i18n/de/libtheme.ts \
	 ../../../i18n/en/libtheme.ts \
	 ../../../i18n/es/libtheme.ts \
	 ../../../i18n/fr/libtheme.ts \
	 ../../../i18n/hu/libtheme.ts \
	 ../../../i18n/ja/libtheme.ts \
	 ../../../i18n/ko/libtheme.ts \
	 ../../../i18n/no/libtheme.ts \
	 ../../../i18n/pl/libtheme.ts \
	 ../../../i18n/pt/libtheme.ts \
	 ../../../i18n/pt_BR/libtheme.ts \
	 ../../../i18n/sl/libtheme.ts \
	 ../../../i18n/zh_CN/libtheme.ts \
	 ../../../i18n/zh_TW/libtheme.ts
