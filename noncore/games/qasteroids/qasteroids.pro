TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= ledmeter.h  sprites.h  toplevel.h  view.h
SOURCES		= ledmeter.cpp  toplevel.cpp  view.cpp main.cpp
TARGET		= qasteroids
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/qasteroids.ts \
	 ../../../i18n/da/qasteroids.ts \
	 ../../../i18n/xx/qasteroids.ts \
	 ../../../i18n/en/qasteroids.ts \
	 ../../../i18n/es/qasteroids.ts \
	 ../../../i18n/fr/qasteroids.ts \
	 ../../../i18n/hu/qasteroids.ts \
	 ../../../i18n/ja/qasteroids.ts \
	 ../../../i18n/ko/qasteroids.ts \
	 ../../../i18n/no/qasteroids.ts \
	 ../../../i18n/pl/qasteroids.ts \
	 ../../../i18n/pt/qasteroids.ts \
	 ../../../i18n/pt_BR/qasteroids.ts \
	 ../../../i18n/sl/qasteroids.ts \
	 ../../../i18n/zh_CN/qasteroids.ts \
	 ../../../i18n/zh_TW/qasteroids.ts
