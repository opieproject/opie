TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= security.h
SOURCES		= security.cpp main.cpp
INTERFACES	= securitybase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= security

TRANSLATIONS = ../../../i18n/de/security.ts \
	 ../../../i18n/en/security.ts \
	 ../../../i18n/es/security.ts \
	 ../../../i18n/fr/security.ts \
	 ../../../i18n/hu/security.ts \
	 ../../../i18n/ja/security.ts \
	 ../../../i18n/ko/security.ts \
	 ../../../i18n/no/security.ts \
	 ../../../i18n/pl/security.ts \
	 ../../../i18n/pt/security.ts \
	 ../../../i18n/pt_BR/security.ts \
	 ../../../i18n/sl/security.ts \
	 ../../../i18n/zh_CN/security.ts \
	 ../../../i18n/zh_TW/security.ts \
	 ../../../i18n/da/security.ts
