TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin

SOURCES		= main.cpp

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include .
TARGET		= taboapp
LIBS		+= -lqpe -lopiecore2 -lopieui2 -loapp
INCLUDEPATH	+= $(OPIEDIR)/core/apps/oapp
DEPENDPATH	+= $(OPIEDIR)/core/apps/oapp
MOC_DIR		= .build
OBJECTS_DIR	= .build
UI_DIR		= .build

include ( $(OPIEDIR)/include.pro )
