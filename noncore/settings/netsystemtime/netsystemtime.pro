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


TRANSLATIONS = ../../../i18n/de/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/en/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/es/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/fr/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/hu/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/ja/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/ko/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/no/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/pl/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/pt/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/pt_BR/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/sl/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/zh_CN/../../bin/netsystemtime.ts
TRANSLATIONS += ../../../i18n/zh_TW/../../bin/netsystemtime.ts

