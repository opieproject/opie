TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= settings.h
SOURCES		= light.cpp main.cpp
INTERFACES	= lightsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= light-and-power

TRANSLATIONS = ../../../i18n/de/light-and-power.ts \
	 ../../../i18n/en/light-and-power.ts \
	 ../../../i18n/es/light-and-power.ts \
	 ../../../i18n/fr/light-and-power.ts \
	 ../../../i18n/hu/light-and-power.ts \
	 ../../../i18n/ja/light-and-power.ts \
	 ../../../i18n/ko/light-and-power.ts \
	 ../../../i18n/no/light-and-power.ts \
	 ../../../i18n/pl/light-and-power.ts \
	 ../../../i18n/pt/light-and-power.ts \
	 ../../../i18n/pt_BR/light-and-power.ts \
	 ../../../i18n/sl/light-and-power.ts \
	 ../../../i18n/zh_CN/light-and-power.ts \
	 ../../../i18n/zh_TW/light-and-power.ts
