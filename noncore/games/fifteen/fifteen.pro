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
