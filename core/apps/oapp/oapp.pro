TEMPLATE	= lib
CONFIG		= qt warn_on release

TARGET		= oapp
VERSION		= 1.0.0

LIBS		+= -lqpe -lopie
HEADERS		= \
		  oappplugin.h \
		  oappinterface.h
SOURCES		= \
		  oappplugin.cpp

DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
INCLUDEPATH	+= $(OPIEDIR)/include 
DEPENDPATH	+= $(OPIEDIR)/include .
MOC_DIR		= .build
OBJECTS_DIR	= .build
UI_DIR		= .build

include ( $(OPIEDIR)/include.pro )
