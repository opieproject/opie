TEMPLATE      = lib
CONFIG       += qt warn_on release
HEADERS       = home.h
SOURCES       = home.cpp
TARGET        = homeapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

TRANSLATIONS = ../../../i18n/de/libhomeapplet.ts \
	 ../../../i18n/da/libhomeapplet.ts \
	 ../../../i18n/xx/libhomeapplet.ts \
	 ../../../i18n/en/libhomeapplet.ts \
	 ../../../i18n/es/libhomeapplet.ts \
	 ../../../i18n/fr/libhomeapplet.ts \
	 ../../../i18n/hu/libhomeapplet.ts \
	 ../../../i18n/ja/libhomeapplet.ts \
	 ../../../i18n/ko/libhomeapplet.ts \
	 ../../../i18n/no/libhomeapplet.ts \
	 ../../../i18n/pl/libhomeapplet.ts \
	 ../../../i18n/pt/libhomeapplet.ts \
	 ../../../i18n/pt_BR/libhomeapplet.ts \
	 ../../../i18n/sl/libhomeapplet.ts \
	 ../../../i18n/zh_CN/libhomeapplet.ts \
	 ../../../i18n/zh_TW/libhomeapplet.ts
