DESTDIR		= ../bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= fifteen.h
SOURCES		= fifteen.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= fifteen

TRANSLATIONS = ../i18n/pt_BR/fifteen.ts
TRANSLATIONS   += ../i18n/de/fifteen.ts
TRANSLATIONS   += ../i18n/en/fifteen.ts
TRANSLATIONS   += ../i18n/hu/fifteen.ts
TRANSLATIONS   += ../i18n/ja/fifteen.ts
TRANSLATIONS   += ../i18n/ko/fifteen.ts
TRANSLATIONS   += ../i18n/no/fifteen.ts
TRANSLATIONS   += ../i18n/zh_CN/fifteen.ts
TRANSLATIONS   += ../i18n/zh_TW/fifteen.ts
