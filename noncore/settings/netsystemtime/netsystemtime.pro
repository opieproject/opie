TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= ntp.h settime.h ntpbase.h
SOURCES		= main.cpp ntp.cpp settime.cpp ntpbase.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	= 
DESTDIR         = $(OPIEDIR)/bin
TARGET		= netsystemtime

TRANSLATIONS = ../../../i18n/de/netsystemtime.ts \
	 ../../../i18n/en/netsystemtime.ts \
	 ../../../i18n/es/netsystemtime.ts \
	 ../../../i18n/fr/netsystemtime.ts \
	 ../../../i18n/hu/netsystemtime.ts \
	 ../../../i18n/ja/netsystemtime.ts \
	 ../../../i18n/ko/netsystemtime.ts \
	 ../../../i18n/no/netsystemtime.ts \
	 ../../../i18n/pl/netsystemtime.ts \
	 ../../../i18n/pt/netsystemtime.ts \
	 ../../../i18n/pt_BR/netsystemtime.ts \
	 ../../../i18n/sl/netsystemtime.ts \
	 ../../../i18n/zh_CN/netsystemtime.ts \
	 ../../../i18n/zh_TW/netsystemtime.ts
