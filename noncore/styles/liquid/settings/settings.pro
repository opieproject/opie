TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= liquidset.h
SOURCES		= liquidset.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET		= liquid-settings

TRANSLATIONS = ../../../../i18n/de/liquid-settings.ts \
	 ../../../../i18n/en/liquid-settings.ts \
	 ../../../../i18n/es/liquid-settings.ts \
	 ../../../../i18n/fr/liquid-settings.ts \
	 ../../../../i18n/hu/liquid-settings.ts \
	 ../../../../i18n/ja/liquid-settings.ts \
	 ../../../../i18n/ko/liquid-settings.ts \
	 ../../../../i18n/no/liquid-settings.ts \
	 ../../../../i18n/pl/liquid-settings.ts \
	 ../../../../i18n/pt/liquid-settings.ts \
	 ../../../../i18n/pt_BR/liquid-settings.ts \
	 ../../../../i18n/sl/liquid-settings.ts \
	 ../../../../i18n/zh_CN/liquid-settings.ts \
	 ../../../../i18n/zh_TW/liquid-settings.ts
