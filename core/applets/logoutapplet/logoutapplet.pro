TEMPLATE      = lib
CONFIG       += qt warn_on release
HEADERS       = logout.h
SOURCES       = logout.cpp
TARGET        = logoutapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

TRANSLATIONS = ../../../i18n/de/liblogoutapplet.ts \
	 ../../../i18n/da/liblogoutapplet.ts \
	 ../../../i18n/xx/liblogoutapplet.ts \
	 ../../../i18n/en/liblogoutapplet.ts \
	 ../../../i18n/es/liblogoutapplet.ts \
	 ../../../i18n/fr/liblogoutapplet.ts \
	 ../../../i18n/hu/liblogoutapplet.ts \
	 ../../../i18n/ja/liblogoutapplet.ts \
	 ../../../i18n/ko/liblogoutapplet.ts \
	 ../../../i18n/no/liblogoutapplet.ts \
	 ../../../i18n/pl/liblogoutapplet.ts \
	 ../../../i18n/pt/liblogoutapplet.ts \
	 ../../../i18n/pt_BR/liblogoutapplet.ts \
	 ../../../i18n/sl/liblogoutapplet.ts \
	 ../../../i18n/zh_CN/liblogoutapplet.ts \
	 ../../../i18n/zh_TW/liblogoutapplet.ts
