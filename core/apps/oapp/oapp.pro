TEMPLATE	= lib
CONFIG		= qt warn_on 

TARGET		= oapp
VERSION		= 1.0.0

LIBS		+= -lqpe
HEADERS		= \
		  oappplugin.h \
		  oappinterface.h
SOURCES		= \
		  oappplugin.cpp

DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
INCLUDEPATH	+= $(OPIEDIR)/include 
DEPENDPATH	+= $(OPIEDIR)/include .

include ( $(OPIEDIR)/include.pro )
