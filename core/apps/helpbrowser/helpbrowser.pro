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

TRANSLATIONS = ../../../i18n/de/helpbrowser.ts \
	 ../../../i18n/en/helpbrowser.ts \
	 ../../../i18n/es/helpbrowser.ts \
	 ../../../i18n/fr/helpbrowser.ts \
	 ../../../i18n/hu/helpbrowser.ts \
	 ../../../i18n/ja/helpbrowser.ts \
	 ../../../i18n/ko/helpbrowser.ts \
	 ../../../i18n/no/helpbrowser.ts \
	 ../../../i18n/pl/helpbrowser.ts \
	 ../../../i18n/pt/helpbrowser.ts \
	 ../../../i18n/pt_BR/helpbrowser.ts \
	 ../../../i18n/sl/helpbrowser.ts \
	 ../../../i18n/zh_CN/helpbrowser.ts \
	 ../../../i18n/it/helpbrowser.ts \
	 ../../../i18n/zh_TW/helpbrowser.ts
