TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= mainwindow.h
SOURCES	= main.cpp \
    mainwindow.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET		= datebook2

include ( $(OPIEDIR)/include.pro )
