DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= fifteen.h
SOURCES		= fifteen.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= fifteen

TRANSLATIONS = ../../../i18n/de/fifteen.ts \
	 ../../../i18n/en/fifteen.ts \
	 ../../../i18n/es/fifteen.ts \
	 ../../../i18n/fr/fifteen.ts \
	 ../../../i18n/hu/fifteen.ts \
	 ../../../i18n/ja/fifteen.ts \
	 ../../../i18n/ko/fifteen.ts \
	 ../../../i18n/no/fifteen.ts \
	 ../../../i18n/pl/fifteen.ts \
	 ../../../i18n/pt/fifteen.ts \
	 ../../../i18n/pt_BR/fifteen.ts \
	 ../../../i18n/sl/fifteen.ts \
	 ../../../i18n/zh_CN/fifteen.ts \
	 ../../../i18n/zh_TW/fifteen.ts
