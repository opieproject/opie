TEMPLATE        = app
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin
HEADERS		= mindbreaker.h
SOURCES		= main.cpp \
              mindbreaker.cpp
TARGET          = mindbreaker
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/de/mindbreaker.ts
TRANSLATIONS += ../i18n/pt_BR/mindbreaker.ts
TRANSLATIONS   += ../i18n/en/mindbreaker.ts
TRANSLATIONS   += ../i18n/hu/mindbreaker.ts
TRANSLATIONS   += ../i18n/fr/mindbreaker.ts
TRANSLATIONS   += ../i18n/ja/mindbreaker.ts
TRANSLATIONS   += ../i18n/sl/mindbreaker.ts
TRANSLATIONS   += ../i18n/pl/mindbreaker.ts
TRANSLATIONS   += ../i18n/ko/mindbreaker.ts
TRANSLATIONS   += ../i18n/no/mindbreaker.ts
TRANSLATIONS   += ../i18n/zh_CN/mindbreaker.ts
TRANSLATIONS   += ../i18n/zh_TW/mindbreaker.ts
