TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= helpbrowser.h
SOURCES		= helpbrowser.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 

TRANSLATIONS = ../../../i18n/de/helpbrowser.pro.in.ts \
	 ../../../i18n/en/helpbrowser.pro.in.ts \
	 ../../../i18n/es/helpbrowser.pro.in.ts \
	 ../../../i18n/fr/helpbrowser.pro.in.ts \
	 ../../../i18n/hu/helpbrowser.pro.in.ts \
	 ../../../i18n/ja/helpbrowser.pro.in.ts \
	 ../../../i18n/ko/helpbrowser.pro.in.ts \
	 ../../../i18n/no/helpbrowser.pro.in.ts \
	 ../../../i18n/pl/helpbrowser.pro.in.ts \
	 ../../../i18n/pt/helpbrowser.pro.in.ts \
	 ../../../i18n/pt_BR/helpbrowser.pro.in.ts \
	 ../../../i18n/sl/helpbrowser.pro.in.ts \
	 ../../../i18n/zh_CN/helpbrowser.pro.in.ts \
	 ../../../i18n/zh_TW/helpbrowser.pro.in.ts
