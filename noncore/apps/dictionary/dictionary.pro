TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../bin

HEADERS	= dicttool.h
SOURCES	= dicttool.cpp main.cpp

INTERFACES	= dicttoolbase.ui

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe

TARGET		= dictionary
