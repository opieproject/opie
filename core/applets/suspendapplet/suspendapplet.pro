TEMPLATE      = lib
CONFIG       += qt warn_on release
HEADERS       = suspend.h
SOURCES       = suspend.cpp
TARGET        = suspendapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

TRANSLATIONS = ../../../i18n/de/libsuspendapplet.ts \
	 ../../../i18n/en/libsuspendapplet.ts \
	 ../../../i18n/es/libsuspendapplet.ts \
	 ../../../i18n/fr/libsuspendapplet.ts \
	 ../../../i18n/hu/libsuspendapplet.ts \
	 ../../../i18n/ja/libsuspendapplet.ts \
	 ../../../i18n/ko/libsuspendapplet.ts \
	 ../../../i18n/no/libsuspendapplet.ts \
	 ../../../i18n/pl/libsuspendapplet.ts \
	 ../../../i18n/pt/libsuspendapplet.ts \
	 ../../../i18n/pt_BR/libsuspendapplet.ts \
	 ../../../i18n/sl/libsuspendapplet.ts \
	 ../../../i18n/zh_CN/libsuspendapplet.ts \
	 ../../../i18n/zh_TW/libsuspendapplet.ts
