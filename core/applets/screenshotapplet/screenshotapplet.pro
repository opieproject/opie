TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   screenshot.h inputDialog.h screenshotappletimpl.h
SOURCES =   screenshot.cpp inputDialog.cpp screenshotappletimpl.cpp
TARGET    = screenshotapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj

TRANSLATIONS = ../../../i18n/de/libscreenshotapplet.ts \
	 ../../../i18n/da/libscreenshotapplet.ts \
	 ../../../i18n/xx/libscreenshotapplet.ts \
	 ../../../i18n/en/libscreenshotapplet.ts \
	 ../../../i18n/es/libscreenshotapplet.ts \
	 ../../../i18n/fr/libscreenshotapplet.ts \
	 ../../../i18n/hu/libscreenshotapplet.ts \
	 ../../../i18n/ja/libscreenshotapplet.ts \
	 ../../../i18n/ko/libscreenshotapplet.ts \
	 ../../../i18n/no/libscreenshotapplet.ts \
	 ../../../i18n/pl/libscreenshotapplet.ts \
	 ../../../i18n/pt/libscreenshotapplet.ts \
	 ../../../i18n/pt_BR/libscreenshotapplet.ts \
	 ../../../i18n/sl/libscreenshotapplet.ts \
	 ../../../i18n/zh_CN/libscreenshotapplet.ts \
	 ../../../i18n/zh_TW/libscreenshotapplet.ts
