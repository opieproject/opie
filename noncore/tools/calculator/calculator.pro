DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= calculatorimpl.h
SOURCES		= calculatorimpl.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= calculator.ui
TARGET		= calculator

TRANSLATIONS    = ../i18n/pt_BR/calculator.ts
TRANSLATIONS   += ../i18n/de/calculator.ts
TRANSLATIONS   += ../i18n/en/calculator.ts
TRANSLATIONS   += ../i18n/hu/calculator.ts
TRANSLATIONS   += ../i18n/ja/calculator.ts
TRANSLATIONS   += ../i18n/fr/calculator.ts
TRANSLATIONS   += ../i18n/ko/calculator.ts
TRANSLATIONS   += ../i18n/no/calculator.ts
TRANSLATIONS   += ../i18n/zh_CN/calculator.ts
TRANSLATIONS   += ../i18n/zh_TW/calculator.ts
