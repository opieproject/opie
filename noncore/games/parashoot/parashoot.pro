TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= interface.h man.h cannon.h base.h bullet.h helicopter.h
SOURCES		= main.cpp interface.cpp man.cpp cannon.cpp base.cpp bullet.cpp helicopter.cpp
TARGET		= parashoot 
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/parashoot.ts \
	 ../../../i18n/en/parashoot.ts \
	 ../../../i18n/es/parashoot.ts \
	 ../../../i18n/fr/parashoot.ts \
	 ../../../i18n/hu/parashoot.ts \
	 ../../../i18n/ja/parashoot.ts \
	 ../../../i18n/ko/parashoot.ts \
	 ../../../i18n/no/parashoot.ts \
	 ../../../i18n/pl/parashoot.ts \
	 ../../../i18n/pt/parashoot.ts \
	 ../../../i18n/pt_BR/parashoot.ts \
	 ../../../i18n/sl/parashoot.ts \
	 ../../../i18n/zh_CN/parashoot.ts \
	 ../../../i18n/zh_TW/parashoot.ts
