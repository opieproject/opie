DESTDIR		= $(QPEDIR)/bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= calculatorimpl.h
SOURCES		= calculatorimpl.cpp \
		  main.cpp
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe
INTERFACES	= calculator.ui
TARGET		= calculator

TRANSLATIONS = ../i18n/de/calculator.ts
