TEMPLATE        = app

CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin

HEADERS		= showimg.h

SOURCES		= main.cpp \
		  showimg.cpp

TARGET          = showimg

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

REQUIRES        = showimg

TRANSLATIONS    = ../i18n/pt_BR/showimg.ts
TRANSLATIONS    += ../i18n/es/showimg.ts
TRANSLATIONS    += ../i18n/pt/showimg.ts
TRANSLATIONS   += ../i18n/de/showimg.ts
TRANSLATIONS   += ../i18n/en/showimg.ts
TRANSLATIONS   += ../i18n/hu/showimg.ts
TRANSLATIONS   += ../i18n/sl/showimg.ts
TRANSLATIONS   += ../i18n/ja/showimg.ts
TRANSLATIONS   += ../i18n/pl/showimg.ts
TRANSLATIONS   += ../i18n/ko/showimg.ts
TRANSLATIONS   += ../i18n/no/showimg.ts
TRANSLATIONS   += ../i18n/zh_CN/showimg.ts
TRANSLATIONS   += ../i18n/zh_TW/showimg.ts
TRANSLATIONS   += ../i18n/fr/showimg.ts
