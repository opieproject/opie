TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= today.h todaybase.h todayconfig.h clickablelabel.h
SOURCES		= main.cpp today.cpp todaybase.cpp todayconfig.cpp clickablelabel.cpp

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	= 
TARGET		= today
DESTDIR		= $(OPIEDIR)/bin
TRANSLATIONS    = ../i18n/pt_BR/today.ts
