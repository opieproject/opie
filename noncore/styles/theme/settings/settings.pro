TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= themeset.h
SOURCES		= themeset.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= theme-settings

TRANSLATIONS = ../../../../i18n/de/theme-settings.ts \
	 ../../../../i18n/en/theme-settings.ts \
	 ../../../../i18n/es/theme-settings.ts \
	 ../../../../i18n/fr/theme-settings.ts \
	 ../../../../i18n/hu/theme-settings.ts \
	 ../../../../i18n/ja/theme-settings.ts \
	 ../../../../i18n/ko/theme-settings.ts \
	 ../../../../i18n/no/theme-settings.ts \
	 ../../../../i18n/pl/theme-settings.ts \
	 ../../../../i18n/pt/theme-settings.ts \
	 ../../../../i18n/pt_BR/theme-settings.ts \
	 ../../../../i18n/sl/theme-settings.ts \
	 ../../../../i18n/zh_CN/theme-settings.ts \
	 ../../../../i18n/zh_TW/theme-settings.ts
