TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= wordgame.h
SOURCES		= main.cpp \
		  wordgame.cpp
TARGET		= oyatzee
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/oyatzee.ts \
	 ../../../i18n/nl/oyatzee.ts \
	 ../../../i18n/da/oyatzee.ts \
	 ../../../i18n/xx/oyatzee.ts \
	 ../../../i18n/en/oyatzee.ts \
	 ../../../i18n/es/oyatzee.ts \
	 ../../../i18n/fr/oyatzee.ts \
	 ../../../i18n/hu/oyatzee.ts \
	 ../../../i18n/ja/oyatzee.ts \
	 ../../../i18n/ko/oyatzee.ts \
	 ../../../i18n/no/oyatzee.ts \
	 ../../../i18n/pl/oyatzee.ts \
	 ../../../i18n/pt/oyatzee.ts \
	 ../../../i18n/pt_BR/oyatzee.ts \
	 ../../../i18n/sl/oyatzee.ts \
	 ../../../i18n/zh_CN/oyatzee.ts \
	 ../../../i18n/zh_TW/oyatzee.ts



include ( $(OPIEDIR)/include.pro )
