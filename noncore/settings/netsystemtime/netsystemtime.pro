TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= ntp.h settime.h
SOURCES		= main.cpp ntp.cpp settime.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	= ntpbase.ui
DESTDIR         = $(OPIEDIR)/bin
TARGET		= netsystemtime


TRANSLATIONS = ../../../i18n/de/netsystemtime.ts
TRANSLATIONS += ../../../i18n/en/netsystemtime.ts
TRANSLATIONS += ../../../i18n/es/netsystemtime.ts
TRANSLATIONS += ../../../i18n/fr/netsystemtime.ts
TRANSLATIONS += ../../../i18n/hu/netsystemtime.ts
TRANSLATIONS += ../../../i18n/ja/netsystemtime.ts
TRANSLATIONS += ../../../i18n/ko/netsystemtime.ts
TRANSLATIONS += ../../../i18n/no//netsystemtime.ts
TRANSLATIONS += ../../../i18n/pl/netsystemtime.ts
TRANSLATIONS += ../../../i18n/pt/netsystemtime.ts
TRANSLATIONS += ../../../i18n/pt_BR/netsystemtime.ts
TRANSLATIONS += ../../../i18n/sl/netsystemtime.ts
TRANSLATIONS += ../../../i18n/zh_CN/netsystemtime.ts
TRANSLATIONS += ../../../i18n/zh_TW/netsystemtime.ts

