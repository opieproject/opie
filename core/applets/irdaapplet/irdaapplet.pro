TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   irda.h irdaappletimpl.h
SOURCES =   irda.cpp irdaappletimpl.cpp
TARGET    = irdaapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

TRANSLATIONS = ../../../i18n/de/libirdaapplet.ts \
	 ../../../i18n/da/libirdaapplet.ts \
	 ../../../i18n/xx/libirdaapplet.ts \
	 ../../../i18n/en/libirdaapplet.ts \
	 ../../../i18n/es/libirdaapplet.ts \
	 ../../../i18n/fr/libirdaapplet.ts \
	 ../../../i18n/hu/libirdaapplet.ts \
	 ../../../i18n/ja/libirdaapplet.ts \
	 ../../../i18n/ko/libirdaapplet.ts \
	 ../../../i18n/no/libirdaapplet.ts \
	 ../../../i18n/pl/libirdaapplet.ts \
	 ../../../i18n/pt/libirdaapplet.ts \
	 ../../../i18n/pt_BR/libirdaapplet.ts \
	 ../../../i18n/sl/libirdaapplet.ts \
	 ../../../i18n/zh_CN/libirdaapplet.ts \
	 ../../../i18n/zh_TW/libirdaapplet.ts
