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
TARGET		= systemtime

TRANSLATIONS = ../../../i18n/de/systemtime.ts \
	 ../../../i18n/en/systemtime.ts \
	 ../../../i18n/es/systemtime.ts \
	 ../../../i18n/fr/systemtime.ts \
	 ../../../i18n/hu/systemtime.ts \
	 ../../../i18n/ja/systemtime.ts \
	 ../../../i18n/ko/systemtime.ts \
	 ../../../i18n/no/systemtime.ts \
	 ../../../i18n/pl/systemtime.ts \
	 ../../../i18n/pt/systemtime.ts \
	 ../../../i18n/pt_BR/systemtime.ts \
	 ../../../i18n/sl/systemtime.ts \
	 ../../../i18n/zh_CN/systemtime.ts \
	 ../../../i18n/zh_TW/systemtime.ts
