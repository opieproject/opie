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

TRANSLATIONS = ../i18n/de/calculator.ts
TRANSLATIONS += ../i18n/pt_BR/calculator.ts
